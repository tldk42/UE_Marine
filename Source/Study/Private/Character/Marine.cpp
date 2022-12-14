#include "Character/Marine.h"

#include "AIHelpers.h"
#include "Item/Item.h"
#include "Item/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AMarine::AMarine()
	:
	CameraCurrentFOV(0.f),
	CameraDefaultFOV(0.f),
	CameraZoomFOV(35.f),
	ZoomInterpSpeed(20.f),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	bAiming(false),
	CrosshairShootingFactor(0.f),
	CrosshairSpreadMultiplier(0.f),
	CrosshairAimFactor(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	ShootTimeDuration(0.05f),
	bFiring(false),
	bShouldTraceForItems(false),
	Starting5mmAmmo(10),
	Starting7mmAmmo(10),
	Starting9mmAmmo(10)
{
	PrimaryActorTick.bCanEverTick = true;

	LastInput = FVector2D::ZeroVector;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true; // To follow Controller ( 캐릭터가 움직이는 방향으로 따라감)
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // 카메라 자체는 회전 X 카메라를 연결하는 조인트만 회전

	// Don't rotate when the controller rotates. let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));
}

// Called when the game starts or when spawned
void AMarine::BeginPlay()
{
	Super::BeginPlay();

	if (Camera)
	{
		CameraCurrentFOV = Camera->FieldOfView;
		CameraDefaultFOV = CameraCurrentFOV;
	}
	HandR = SpawnDefaultWeapon();
	EquipWeapon(HandR);

	InitializeAmmoMap();
}

void AMarine::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

	TurnInPlaceDirection = BaseTurnRate * Rate;
}

void AMarine::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMarine::DropBtnPressed()
{
	DetachWeapon();
}

void AMarine::DropBtnReleased()
{
}

void AMarine::SwapBtnPressed()
{
	if (TraceHitItem)
	{
		auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
		if (TraceHitWeapon)
		{
			SwapWeapon(TraceHitWeapon);
		}
	}
}

bool AMarine::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// Get Viewport size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector   CrosshairWorldLocation;
	FVector   CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldLocation,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		const FVector Start{CrosshairWorldLocation};
		const FVector End{Start + CrosshairWorldDirection * 50'000.f};
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AMarine::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector    HitLocation;

		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (TraceHitItem)
			{
				TraceHitItem->ShowWidget();
			}

			if (LastTracedItem)
			{
				if (TraceHitItem != LastTracedItem)
				{
					LastTracedItem->HideWidget();
				}
			}
			LastTracedItem = TraceHitItem;
		}
	}
	else if (LastTracedItem)
	{
		LastTracedItem->HideWidget();
		LastTracedItem = nullptr;
	}
}

AWeapon* AMarine::SpawnDefaultWeapon() const
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

//TODO: 양손무기를 사용하는데 메서드들이 중구난방임 더 체계적인 방식 필요
void AMarine::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		/// 기본 Weapon 클래스는 Ignore가 아닌 Block방식이다.
		/// 이상태로 계속 한다면 플레이어와 계속해서 Blocking되므로
		/// 플레이어가 장착한 무기는 collision반응을 Ignore로 변경해준다. ---> Item -> SetItemProperty로 변경

		const USkeletalMeshSocket* RHandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

		if (RHandSocket)
		{
			RHandSocket->AttachActor(WeaponToEquip, GetMesh());
			HandR = WeaponToEquip;
			HandR->SetItemState(EItemState::EIS_Equipped);
			MRLOG(Warning, TEXT("Weapon R Equipped"));
		}

		///TODO: 양손무기 부착 해결
		/// 양손 무기는 기본 무기를 복제하는 방식으로 왼쪽으로 붙이고 있다
		/// Spawn을 반복하면 새 개체가 생성 소멸 반복 -> 메모리 파편화가능성 높음
		if (WeaponToEquip->IsDualWeapon())
		{
			bDualWeapon = true;
			const USkeletalMeshSocket* LHandSocket = GetMesh()->GetSocketByName(FName("LeftHandSocket"));
			if (LHandSocket)
			{
				auto Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponToEquip->GetClass());
				InitStaticDuplicateObjectParams(WeaponToEquip, Weapon);
				LHandSocket->AttachActor(Weapon, GetMesh());
				HandL = Weapon;
				HandL->SetItemState(EItemState::EIS_Equipped);
				MRLOG(Warning, TEXT("Weapon L Equipped"));
			}
		}
		else
		{
			bDualWeapon = false;
		}
	}
}

void AMarine::DetachWeapon()
{
	FDetachmentTransformRules DetachmentTransformRules(
		EDetachmentRule::KeepWorld,
		true);

	if (HandR)
	{
		HandR->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		HandR->SetItemState(EItemState::EIS_Falling);
		HandR->ThrowWeapon();
		if (HandR->IsDualWeapon())
		{
			if (HandL)
			{
				HandL->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
				//HandL->SetItemState(EItemState::EIS_Falling);
				//HandL->ThrowWeapon();
				// HandL->GetClass()->ConditionalBeginDestroy();
				HandL->Destroy();
			}
			HandL = nullptr;
		}
		HandR = nullptr;
	}
}

void AMarine::SwapWeapon(AWeapon* WeaponToSwap)
{
	DetachWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	LastTracedItem = nullptr;
}

void AMarine::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType2::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType2::EAT_5mm, Starting5mmAmmo);
	AmmoMap.Add(EAmmoType2::EAT_7mm, Starting7mmAmmo);
}

bool AMarine::WeaponHasAmmo() const
{
	if (HandR == nullptr)
		return false;

	return HandR->GetAmmo() > 0;
}

// Called every frame
void AMarine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float len = LastInput.Size();

	if (len > 1.f)
	{
		LastInput /= len;
	}
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), LastInput.Y);
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), LastInput.X);

	LastInput = FVector2D(0.f, 0.f);

	CameraCurrentFOV =
		bAiming
			? FMath::FInterpTo(CameraCurrentFOV, CameraZoomFOV, DeltaTime, ZoomInterpSpeed)
			: FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	Camera->SetFieldOfView(CameraCurrentFOV);

	BaseTurnRate =
		bAiming ? AimingTurnRate : HipTurnRate;
	BaseLookUpRate =
		bAiming ? AimingLookUpRate : HipLookUpRate;

	CalculateCrosshairSpread(DeltaTime);

	TraceForItems();
}

// Called to bind functionality to input
void AMarine::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMarine::Jump);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMarine::ReloadBtnPressed);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMarine::FireBtnPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMarine::FireBtnReleased);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMarine::AimingBtnPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMarine::AimingBtnReleased);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AMarine::DropBtnPressed);
	PlayerInputComponent->BindAction("Drop", IE_Released, this, &AMarine::DropBtnReleased);
	PlayerInputComponent->BindAction("Swap", IE_Pressed, this, &AMarine::SwapBtnPressed);

	PlayerInputComponent->BindAxis("Forward", this, &AMarine::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &AMarine::MoveRight);

	PlayerInputComponent->BindAxis("TurnUpRate", this, &AMarine::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMarine::LookUpRate);
}

void AMarine::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(
		AutomaticFireHandle,
		this,
		&AMarine::AutoFireReset,
		0.1f);
}

void AMarine::AutoFireReset()
{
	CombatState = ECombatState::ECS_UnOccupied;

	if (WeaponHasAmmo())
	{
		if (bFireBtnPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		// Reload Weapon
	}
}

void AMarine::ReloadBtnPressed()
{
	ReloadWeapon();
}

void AMarine::FireBtnPressed()
{
	bFireBtnPressed = true;
	if (WeaponHasAmmo())
	{
		FireWeapon();
	}
}

void AMarine::FireWeapon()
{
	if (HandR == nullptr)
		return;

	if (CombatState != ECombatState::ECS_UnOccupied)
		return;

	if (WeaponHasAmmo())
	{
		// Play Fire Sound
		PlayFireSound();

		// Send Bullet
		SendBullet();

		// Play Hip fire Montage
		PlayHipFireMontage();

		// Decrease Ammo
		HandR->FireAmmo();

		StartFireTimer();
	}
}

void AMarine::PlayFireSound() const
{
	// Play Fire sound
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AMarine::SendBullet()
{
	const USkeletalMeshSocket* BarrelSocket = HandR->GetItemMesh()->GetSocketByName("BarrelSocket_R");

	if (BarrelSocket)
	{
		FVector BeamEnd;
		//const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(HandR->GetItemMesh());

		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (Muzzle_RFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Muzzle_RFlash, SocketTransform);
		}

		if (Muzzle_LFlash && HandR->IsDualWeapon())
		{
			const USkeletalMeshSocket* BarrelSocket2 = HandL->GetItemMesh()->GetSocketByName("BarrelSocket_L");
			const FTransform           SocketTransform2 = BarrelSocket2->GetSocketTransform(HandL->GetItemMesh());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Muzzle_LFlash, SocketTransform2);
		}

		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}

void AMarine::PlayHipFireMontage() const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireMontageDefault && FireMontageRifle)
	{
		if (!bDualWeapon)
		{
			AnimInstance->Montage_Play(FireMontageRifle);
		}
		else
		{
			AnimInstance->Montage_Play(FireMontageDefault);
		}
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AMarine::FireBtnReleased()
{
	bFireBtnPressed = false;
}

void AMarine::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_UnOccupied)
		return;
	if (HandR == nullptr)
		return;

	// 올바른 탄약을 가지고있는지 확인
	if (CarryingAmmo())
	{
		// TODO: 무기 타입에 따른 Enum 생성
		// TODO: switch문

		CombatState = ECombatState::ECS_Reloading;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontageRifle)
		{
			AnimInstance->Montage_Play(ReloadMontageRifle);
			AnimInstance->Montage_JumpToSection(HandR->GetReloadMontage());
		}
	}
}

bool AMarine::CarryingAmmo()
{
	if (HandR == nullptr)
		return false;
	auto AmmoType = HandR->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void AMarine::GrabClip()
{
	if (HandR == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	// Index for the Clip Bone
	int32 ClipBoneIndex{ HandR->GetItemMesh()->GetBoneIndex(HandR->GetClipBoneName())};
	// Clip의 Transform 저장
	ClipTransform = HandR->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentTransformRules, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	HandR->SetMovingClip(true);
}

void AMarine::ReleaseClip()
{
	HandR->SetMovingClip(false);
}

bool AMarine::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	FHitResult CrosshairHitResult;
	bool       bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{
	}
	const FVector StartToEnd{OutBeamLocation - MuzzleSocketLocation};
	FHitResult    WeaponResult;
	GetWorld()->LineTraceSingleByChannel(WeaponResult, MuzzleSocketLocation, MuzzleSocketLocation + StartToEnd * 1.25f, ECC_Visibility);
	if (WeaponResult.bBlockingHit)
	{
		OutBeamLocation = WeaponResult.Location;
		return true;
	}

	return false;
}

void AMarine::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void AMarine::FinishReloading()
{
	if (HandR == nullptr)
		return;

	const auto AmmoType = HandR->GetAmmoType();
	
	// TODO: AmmoMap 업데이트
	if (AmmoMap.Contains(AmmoType))
	{
		int32 CarriedAmmo = AmmoMap[AmmoType];

		const int32 MagEmptySpace = HandR->GetMagazineCapacity() - HandR->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			HandR->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
		}
		else
		{
			HandR->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
		}
		AmmoMap.Add(AmmoType, CarriedAmmo);

	}
	
	CombatState = ECombatState::ECS_UnOccupied;
}

void AMarine::AimingBtnPressed()
{
	bAiming = true;
}

void AMarine::AimingBtnReleased()
{
	bAiming = false;
}

void AMarine::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{0.f, 600.f};
	FVector2D VelocityMultiplierRange{0.f, 1.f};
	FVector   Velocity{GetVelocity()};
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	CrosshairInAirFactor = GetCharacterMovement()->IsFalling()
		                       ? FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f)
		                       : FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);

	CrosshairAimFactor = bAiming
		                     ? FMath::FInterpTo(CrosshairAimFactor, 0.35f, DeltaTime, 30.f)
		                     : FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);

	CrosshairShootingFactor = bFiring
		                          ? FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f)
		                          : FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 60.f);

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor + CrosshairShootingFactor - CrosshairAimFactor;
}


void AMarine::MoveForward(float Value)
{
	LastInput.Y += Value;
}

void AMarine::MoveRight(float Value)
{
	LastInput.X += Value;
}

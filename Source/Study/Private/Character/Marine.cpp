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

void AMarine::FireBtnPressed()
{
	GetWorld()->GetTimerManager().SetTimer(AutomaticFireHandle, this, &AMarine::OnFire, 0.1f, true);
}

void AMarine::OnFire()
{
	bFiring = true;

	auto turnRate = FMath::RandRange(-0.08f, 0.08f);
	TurnAtRate(turnRate);
	bAiming ? LookUpRate(-0.8f) : LookUpRate(-0.15f);

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("Muzzle_01");
	const USkeletalMeshSocket* BarrelSocket2 = GetMesh()->GetSocketByName("Muzzle_02");
	if (BarrelSocket && BarrelSocket2)
	{
		FVector          BeamEnd;
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		const FTransform SocketTransform2 = BarrelSocket2->GetSocketTransform(GetMesh());
		bool             bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (Muzzle_LFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Muzzle_LFlash, SocketTransform);
		}

		if (Muzzle_RFlash && bDualWeapon)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Muzzle_RFlash, SocketTransform2);
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
		/*
		// FHitResult    FireHit;
		// const FVector TraceStart{SocketTransform.GetLocation()};
		// const FQuat   Rotation{SocketTransform.GetRotation()};
		// const FVector RotationAxis{Rotation.GetAxisX()};
		// const FVector TraceEnd {TraceStart + RotationAxis * 50'000.f};
		//
		// FVector BeamEndPoint {TraceEnd};
		//
		// GetWorld()->LineTraceSingleByChannel(FireHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
		// if (FireHit.bBlockingHit)
		// {
		// 	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.f);
		// 	BeamEndPoint = FireHit.Location;
		// 	
		// 	if (ImpactParticles)
		// 	{
		// 		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.Location);
		// 	}
		// }
		//
		// if (BeamParticles)
		// {
		// 	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
		// 	if (Beam)
		// 	{
		// 		Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
		// 	}
		// }

		*/
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireMontage)
	{
		AnimInstance->Montage_Play(FireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AMarine::FireBtnReleased, 0.1f, false);
}

void AMarine::FireBtnReleased()
{
	bFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(AutomaticFireHandle);
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

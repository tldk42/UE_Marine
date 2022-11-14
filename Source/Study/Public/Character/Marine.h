#pragma once

#include "Study.h"
#include "EAmmoType2.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "Marine.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_UnOccupied UMETA(DisplayName = "UnOcuupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};



UCLASS()
class STUDY_API AMarine : public ACharacter
{
	GENERATED_BODY()

public:
	AMarine();

protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	#pragma region Input

	void StartFireTimer();
	void AutoFireReset();
	
	void AimingBtnPressed();
	void AimingBtnReleased();
	void FireBtnPressed();
	void FireBtnReleased();
	void FireWeapon();
	void PlayFireSound() const;
	void SendBullet();
	void PlayHipFireMontage() const;

	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate); // Called via input to turn at a given rate 
	void LookUpRate(float Rate); // Called via input to look up/down at a given rate

	void DropBtnPressed();
	void DropBtnReleased();

	void SwapBtnPressed();

	#pragma endregion Input

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	void CalculateCrosshairSpread(float DeltaTime);
	
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	void TraceForItems();

	class AWeapon* SpawnDefaultWeapon() const;
	void EquipWeapon(AWeapon* WeaponToEquip);
	void DetachWeapon();
	void SwapWeapon(AWeapon* WeaponToSwap);

	void InitializeAmmoMap();

	bool WeaponHasAmmo() const;
public:
	void IncrementOverlappedItemCount(int8 Amount);

#pragma region GETTER

	FORCEINLINE float GetTurnRate()const
	{
		return TurnInPlaceDirection;
	}
	
	FORCEINLINE bool IsDualWeapon() const
	{
		return bDualWeapon;
	}

	FORCEINLINE bool IsAiming() const
	{
		return bAiming;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCrosshairSpreadMultiplier() const
	{
		return CrosshairSpreadMultiplier;
	}

	FORCEINLINE int8 GetOverlappedItemCount() const
	{
		return OverlappedItemCount;
	}


#pragma endregion GETTER

private:
	#pragma region Camera
	// 카메라 포지션을 잡기 위한 springarm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// 캐릭터에 붙어다닐 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	float CameraCurrentFOV;

	float CameraDefaultFOV;

	float CameraZoomFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshairs", meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float TurnInPlaceDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;
	#pragma endregion Camera

	#pragma region Combat

	#pragma region Particles

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Effect", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* Muzzle_LFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Effect", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* Muzzle_RFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Effect", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Effect", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	#pragma endregion Particles

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | State", meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	AWeapon* HandR;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	AWeapon* HandL;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Ammo", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType2, int32> AmmoMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Starting5mmAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Starting7mmAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Effect", meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Effect", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FireMontage;

	FTimerHandle CrosshairShootTimer;

	FTimerHandle AutomaticFireHandle;

	float ShootTimeDuration;

	bool bDualWeapon;				

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	bool bFireBtnPressed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bFiring;


	#pragma endregion Combat

	FVector2D LastInput;

	#pragma region TraceItem

	// True if we should trace every frame for itmes
	bool bShouldTraceForItems;

	// Number of overlapped item
	int8 OverlappedItemCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class AItem* LastTracedItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	#pragma endregion TraceItem
};

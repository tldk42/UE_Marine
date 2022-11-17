#pragma once

#include "Study.h"
#include "Item.h"
#include "AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	EWT_Blast UMETA(DisplayName = "Blast"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),

	EWT_MAX UMETA(DisplayName = "Default Max")
};

/**
 * 
 */
UCLASS()
class STUDY_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

public:
	void ThrowWeapon();

	void FireAmmo();

	void ReloadAmmo(int32 Amount);

#pragma region GETTER PROPERTIES
	FORCEINLINE bool IsDualWeapon() const
	{
		return bDualWeapon;
	}

	FORCEINLINE int32 GetAmmo() const
	{
		return Ammo;
	}

	FORCEINLINE int32 GetMagazineCapacity() const
	{
		return MagazineCapaticy;
	}

	FORCEINLINE EWeaponType GetWeaponType() const
	{
		return WeaponType;
	}

	FORCEINLINE EAmmoType2 GetAmmoType() const
	{
		return AmmoType;
	}

	FORCEINLINE FName GetReloadMontage() const
	{
		return ReloadMontageSection;
	}

	FORCEINLINE FName GetClipBoneName() const
	{
		return ClipBoneName;
	}

	FORCEINLINE void SetMovingClip(bool Value)
	{
		bMovingClip = Value;
	}
#pragma endregion GETTER PROPERTIES

protected:
	virtual void Tick(float DeltaSeconds) override;

	void StopFalling();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName ClipBoneName;
	
	FTimerHandle ThrowWeaponTimer;
	float        ThrowWeaponTime;
	bool         bFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool         bMovingClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Ammo", meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapaticy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType2 AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Montage", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bDualWeapon;
};

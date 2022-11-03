#pragma once

#include "Study.h"
#include "Item.h"
#include "Weapon.generated.h"

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


	FORCEINLINE bool IsDualWeapon() const
	{
		return bDualWeapon;
	}

	FORCEINLINE int32 GetAmmo() const
	{
		return Ammo;
	}

protected:
	virtual void Tick(float DeltaSeconds) override;

	void StopFalling();

private:
	FTimerHandle ThrowWeaponTimer;
	float        ThrowWeaponTime;
	bool         bFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type", meta = (AllowPrivateAccess = "true"))
	bool bDualWeapon;
};

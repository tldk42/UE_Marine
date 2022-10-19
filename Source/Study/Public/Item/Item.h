#pragma once

#include "Study.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_UnCommon UMETA(DisplayName = "UnCommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipeInterping UMETA(DisplayName = "EquipeInterping"),
	EIS_Pickedup UMETA(DisplayName = "Pickedup"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
	  
	EIS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class STUDY_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Actor, UPrimitiveComponent* OtherComp, int32 OhterBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Actor, UPrimitiveComponent* OtherComp, int32 OhterBodyIndex);

	void SetActiveStar();
	void SetItemProperties(EItemState State);
	
public:	
	virtual void Tick(float DeltaTime) override;

	void HideWidget() const;
	void ShowWidget() const;

public:

	#pragma region GETTERPROPERTY
	
	FORCEINLINE USphereComponent* GetAreaSphere() const
	{
		return AreaSphere;
	}

	FORCEINLINE UBoxComponent* GetCollisionBox() const
	{
		return CollisionBox;
	}

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const
	{
		return ItemMesh;
	}

	FORCEINLINE EItemState GetItemState() const
	{
		return ItemState;
	}

	
	#pragma endregion GETTERPROPERTY

	#pragma region SETTERPROPERTY

	 void SetItemState(EItemState State);
	

	#pragma endregion SETTERPROPERTY

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	int32 ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))	
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))	
	EItemState ItemState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))	
	TArray<bool> ActiveStars;
};

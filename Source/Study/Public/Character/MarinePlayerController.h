// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MarinePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API AMarinePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	AMarinePlayerController();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HUDOverlayClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Study.h"
#include "GameFramework/GameModeBase.h"
#include "StudyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STUDY_API AStudyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> Widget;

public:

	UFUNCTION(BlueprintCallable, Category = UIFuncs)
	void ButtonClicked()
	{
		MRLOG_S(Warning);
	}
	
	virtual void BeginPlay() override;
};

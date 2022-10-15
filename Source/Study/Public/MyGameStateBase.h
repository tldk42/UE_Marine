// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Study.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

/**
 * --------------GameState--------------
 * 모든 플레이어가 공유해야 하는 정보를 담당
 * GameMode에 국한된다.
 */
UCLASS()
class STUDY_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMyGameStateBase();

	UFUNCTION()
	void SetScore(int32 NewScore);

	UFUNCTION()
	int32 GetScore() const;

private:
	int32 CurrentScore;
};

// Copyright Epic Games, Inc. All Rights Reserved.


#include "StudyGameModeBase.h"

#include "MyActor.h"
#include "Blueprint/UserWidget.h"


void AStudyGameModeBase::DestroyActorFunction()
{
}

void AStudyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// if(Widget)
	// {
	// 	UUserWidget* Menu =
	// 		CreateWidget<UUserWidget>(GetWorld(), Widget);
	// 	if (Menu)
	// 	{
	// 		Menu->AddToViewport();
	// 		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	// 	}
	// }
	
	
	// GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Actor Spawning"));
	//
	// // 원점
	// FTransform SpawnLocation;
	//
	// SpawnedActor = GetWorld()->SpawnActor<AMyActor>(AMyActor::StaticClass(), SpawnLocation);
	//
	// FTimerHandle Timer;
	// GetWorldTimerManager().SetTimer(Timer, this, &AStudyGameModeBase::DestroyActorFunction, 10);
	//
	// MRLOG_S(Warning);
}

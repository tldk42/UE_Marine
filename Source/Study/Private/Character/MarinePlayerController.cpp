// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MarinePlayerController.h"
#include "Blueprint/UserWidget.h"

void AMarinePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}	
}

AMarinePlayerController::AMarinePlayerController()
{
}

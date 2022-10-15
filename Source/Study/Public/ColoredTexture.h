#pragma once

#include "ColoredTexture.generated.h"

USTRUCT(Blueprintable)
struct STUDY_API FColoredTexture
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HUD)
	UTexture* Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HUD)
	FLinearColor Color;
};

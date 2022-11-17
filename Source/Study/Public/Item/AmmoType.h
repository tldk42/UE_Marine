#pragma once

UENUM(BlueprintType)
enum class EAmmoType2 : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_5mm UMETA(DisplayName = "5.56mm"),
	EAT_7mm UMETA(DisplayName = "7.62mm"),

	EAT_MAX UMETA(DisplayName = "DefaultMax")
};
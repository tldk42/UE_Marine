// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Study.h"
#include "UObject/NoExportTypes.h"
#include "ColoredTexture.h"
#include "Status.h"

// *.generated.h 파일은 언제나 제일 마지막에 포함된다.
#include "UserProfile.generated.h"

/**
 * UCLASS 키워드
 *
 * Blueprintable
 * BlueprintType
 * NotBlueprintType
 */
UCLASS(Blueprintable, BlueprintType)
class STUDY_API UUserProfile : public UObject
{
	GENERATED_BODY()

public:
	/*
	 * UPROPERTY 키워드
	 *
	 * EditAnywhere : 블루프린트, 레벨 내의 각 인스턴스에서 수정할 수도있음
	 *	- EditDefaultsOnly : 블루프린트에서만 가능
	 *	- EditInstanceOnly : 인스턴스에서만 가능
	 *	BlueprintReadWrite : 무조건 Public 멤버여야한다.
	 *	- BlueprintReadOnly : 읽기만 가능 (C++에서 설정가능)
	 *	Category : 정리된 상태 유지
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	float Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	float HpMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	FString Name;

	// UObject로 부터 파생된 클래스 출력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Unit)
	TSubclassOf<UObject> UClassOfPlayer;

	// GameMode C++ 클래스를 상속받은 UCLASS들을 출력
	UPROPERTY(EditAnywhere, meta=(MetaClass="GameMode"), Category=Unit)
	FSoftClassPath UClassGameMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	TEnumAsByte<Status> Status;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	FColoredTexture Texture;
};
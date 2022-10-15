// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Study.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class STUDY_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyActor();

	UPROPERTY()
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Properties)
	FString Name;

	UFUNCTION(BlueprintCallable, Category=Properties)
	FString ToString();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};

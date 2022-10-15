#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RandomMovement.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDY_API URandomMovement : public UActorComponent
{
	GENERATED_BODY()

public:	
	URandomMovement();

	UPROPERTY()
	float MovementRadius;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

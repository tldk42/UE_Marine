

#include "RandomMovement.h"

URandomMovement::URandomMovement()
{
	PrimaryComponentTick.bCanEverTick = true;

	MovementRadius = 5;
}


void URandomMovement::BeginPlay()
{
	Super::BeginPlay();

	
}


void URandomMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Parent = GetOwner();
	if(Parent)
	{
		auto NewPos = Parent->GetActorLocation()
			+ FVector(
				FMath::FRandRange(-1.f, 1.f) * MovementRadius,
				FMath::FRandRange(-1.f, 1.f) * MovementRadius,
				FMath::FRandRange(-1.f, 1.f) * MovementRadius);
		Parent->SetActorLocation(NewPos);
	}

}


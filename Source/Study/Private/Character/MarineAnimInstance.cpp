#include "Character/MarineAnimInstance.h"
#include "Character/Marine.h"
#include "Kismet/KismetMathLibrary.h"

void UMarineAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (MarineCharacter == nullptr)
	{
		MarineCharacter = Cast<AMarine>(TryGetPawnOwner());
	}

	if (MarineCharacter)
	{
		// Get character speed from velocity
		FVector Velocity{MarineCharacter->GetVelocity()};
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// Is Character in the air
		bIsInAir = MarineCharacter->GetCharacterMovement()->IsFalling();

		// Is Character Accelerating
		bIsAccelerating =
			MarineCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

		FRotator AimRotation = MarineCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MarineCharacter->GetVelocity());

		MovementOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		
		if (MarineCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffset = MovementOffset;
		}
		bAiming = MarineCharacter->IsAiming();
	}
	
}

void UMarineAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MarineCharacter = Cast<AMarine>(TryGetPawnOwner());
}

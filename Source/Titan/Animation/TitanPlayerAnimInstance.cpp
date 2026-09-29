#include "Animation/TitanPlayerAnimInstance.h"

#include "GameFramework/Character.h"

void UTitanPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(TryGetPawnOwner());
}

void UTitanPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character)
	{
		return;
	}

	const FVector Velocity = Character->GetVelocity();
	GroundSpeed = Velocity.Size2D();

	// Keep the last direction while standing still so the blend space does not snap to forward
	if (GroundSpeed > KINDA_SMALL_NUMBER)
	{
		const FVector LocalVelocity = Character->GetActorRotation().UnrotateVector(Velocity);
		MoveDirection = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
	}
}

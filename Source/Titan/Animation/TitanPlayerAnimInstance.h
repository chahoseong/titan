#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TitanPlayerAnimInstance.generated.h"

class ACharacter;

/**
 * Computes the locomotion values the player's animation blueprint reads.
 */
UCLASS()
class TITAN_API UTitanPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> Character;

	// Horizontal speed in cm/s
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed = 0.0f;

	// Movement direction relative to the character's facing, in degrees (-180 to 180)
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float MoveDirection = 0.0f;
};

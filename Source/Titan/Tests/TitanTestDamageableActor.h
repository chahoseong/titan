#pragma once

#include "CoreMinimal.h"
#include "Combat/TitanDamageable.h"
#include "GameFramework/Actor.h"
#include "TitanTestDamageableActor.generated.h"

/** For automation tests only: an actor that records the damage it receives. The game does not use it */
UCLASS(NotBlueprintable, NotPlaceable)
class ATitanTestDamageableActor : public AActor, public ITitanDamageable
{
	GENERATED_BODY()

public:
	virtual void ReceiveDamage(float Damage, const FHitResult& Hit) override;

	/** Sum of the damage received so far */
	float ReceivedDamage = 0.0f;
};

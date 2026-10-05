#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TitanDamageable.generated.h"

struct FHitResult;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTitanDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by actors that attacks can damage. The attacker only delivers the damage
 * and the hit; the actor decides how the damage is applied.
 */
class TITAN_API ITitanDamageable
{
	GENERATED_BODY()

public:
	virtual void ReceiveDamage(float Damage, const FHitResult& Hit) = 0;
};

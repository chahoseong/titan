#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TitanBodyPartComponent.generated.h"

class UTitanBodyPartSet;
class UTitanHealthComponent;

/**
 * Gives the owning actor body parts. Resolves a hit to the body part it landed on
 * and scales damage by that body part's damage multiplier.
 */
UCLASS()
class TITAN_API UTitanBodyPartComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTitanBodyPartComponent();

	/** Body part that the hit landed on, or an empty tag if the hit is on no body part */
	FGameplayTag GetHitLocation(const FHitResult& Hit) const;

	/**
	 * Applies Damage times the damage multiplier of the hit location to Health.
	 * Applies nothing if the hit is on no body part.
	 */
	void ApplyDamage(UTitanHealthComponent& Health, float Damage, const FHitResult& Hit) const;

	const UTitanBodyPartSet* GetBodyPartSet() const { return BodyPartSet; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Body Part")
	TObjectPtr<UTitanBodyPartSet> BodyPartSet;
};

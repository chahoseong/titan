#pragma once

#include "CoreMinimal.h"
#include "Characters/TitanCharacter.h"
#include "GameplayTagContainer.h"
#include "TitanBossCharacter.generated.h"

class UTitanBodyPartSet;
class UTitanHealthComponent;

UCLASS(Abstract)
class TITAN_API ATitanBossCharacter : public ATitanCharacter
{
	GENERATED_BODY()

public:
	ATitanBossCharacter();

	/** Body part that the hit landed on, or an empty tag if the hit is not on one of this boss's body parts */
	FGameplayTag GetHitLocation(const FHitResult& Hit) const;

	const UTitanBodyPartSet* GetBodyPartSet() const { return BodyPartSet; }

	UTitanHealthComponent* GetHealthComponent() const { return HealthComponent; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Health")
	TObjectPtr<UTitanHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Body Part")
	TObjectPtr<UTitanBodyPartSet> BodyPartSet;
};

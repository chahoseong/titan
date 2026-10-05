#pragma once

#include "CoreMinimal.h"
#include "Characters/TitanCharacter.h"
#include "Combat/TitanDamageable.h"
#include "TitanBossCharacter.generated.h"

class UTitanBodyPartComponent;
class UTitanHealthComponent;

UCLASS(Abstract)
class TITAN_API ATitanBossCharacter : public ATitanCharacter, public ITitanDamageable
{
	GENERATED_BODY()

public:
	ATitanBossCharacter();

	virtual void ReceiveDamage(float Damage, const FHitResult& Hit) override;

	UTitanHealthComponent* GetHealthComponent() const { return HealthComponent; }
	UTitanBodyPartComponent* GetBodyPartComponent() const { return BodyPartComponent; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Health")
	TObjectPtr<UTitanHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, Category = "Body Part")
	TObjectPtr<UTitanBodyPartComponent> BodyPartComponent;
};

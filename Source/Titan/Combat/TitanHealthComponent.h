#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TitanHealthComponent.generated.h"

/**
 * Health of the owning actor. Damage reduces health, and the owner is dead
 * once health reaches zero.
 */
UCLASS()
class TITAN_API UTitanHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTitanHealthComponent();

	virtual void InitializeComponent() override;

	/**
	 * Reduces health by Damage, down to zero at most.
	 * Does nothing if Damage is not positive or the owner is dead.
	 */
	void ApplyDamage(float Damage);

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }

	bool IsDead() const { return Health <= 0.0f; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	// Set to MaxHealth when the component is initialized
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Health")
	float Health = 0.0f;
};

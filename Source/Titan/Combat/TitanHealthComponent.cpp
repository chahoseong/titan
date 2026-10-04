#include "Combat/TitanHealthComponent.h"

UTitanHealthComponent::UTitanHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Runs before any actor's BeginPlay, so health is ready for code that reads it there
	bWantsInitializeComponent = true;
}

void UTitanHealthComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Health = MaxHealth;
}

void UTitanHealthComponent::ApplyDamage(float Damage)
{
	if (Damage <= 0.0f || IsDead())
	{
		return;
	}

	Health = FMath::Max(Health - Damage, 0.0f);
}

#include "Combat/TitanBodyPartComponent.h"

#include "Combat/TitanBodyPartSet.h"
#include "Combat/TitanHealthComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/HitResult.h"

static TAutoConsoleVariable<bool> CVarShowHits(
	TEXT("Titan.Debug.ShowHits"),
	false,
	TEXT("Show the hit location and the applied damage where each hit lands."));

UTitanBodyPartComponent::UTitanBodyPartComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FGameplayTag UTitanBodyPartComponent::GetHitLocation(const FHitResult& Hit) const
{
	const FTitanBodyPartEntry* BodyPart = BodyPartSet ? BodyPartSet->FindBodyPart(Hit.BoneName) : nullptr;

	return BodyPart ? BodyPart->BodyPart : FGameplayTag();
}

void UTitanBodyPartComponent::ApplyDamage(UTitanHealthComponent& Health, float Damage, const FHitResult& Hit) const
{
	// Damage only gets in through a body part
	const FTitanBodyPartEntry* BodyPart = BodyPartSet ? BodyPartSet->FindBodyPart(Hit.BoneName) : nullptr;
	if (!BodyPart || Health.IsDead())
	{
		return;
	}

	const float AppliedDamage = Damage * BodyPart->DamageMultiplier;
	Health.ApplyDamage(AppliedDamage);

#if ENABLE_DRAW_DEBUG
	if (CVarShowHits.GetValueOnGameThread())
	{
		const FString Text = FString::Printf(TEXT("%s  %.1f"), *BodyPart->BodyPart.ToString(), AppliedDamage);
		DrawDebugString(GetWorld(), Hit.ImpactPoint, Text, nullptr, FColor::Yellow, 1.5f, true, 1.5f);
	}
#endif
}

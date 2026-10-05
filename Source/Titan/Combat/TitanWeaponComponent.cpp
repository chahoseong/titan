#include "Combat/TitanWeaponComponent.h"

#include "Combat/TitanDamageable.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TitanCollisionChannels.h"

UTitanWeaponComponent::UTitanWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTitanWeaponComponent::Fire(const FVector& TargetPoint)
{
	const UWorld* World = GetWorld();
	const double Now = World->GetTimeSeconds();
	if (LastFireTime.IsSet() && Now - LastFireTime.GetValue() < FireInterval)
	{
		return;
	}

	const FVector TraceStart = GetMuzzleLocation();
	const FVector Direction = (TargetPoint - TraceStart).GetSafeNormal();
	if (Direction.IsZero())
	{
		return;
	}

	LastFireTime = Now;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TitanWeaponFire));
	QueryParams.AddIgnoredActor(GetOwner());

	// The shot goes past the target point up to the range, and stops at the first thing it hits
	FHitResult Hit;
	if (!World->LineTraceSingleByChannel(Hit, TraceStart, TraceStart + Direction * Range, TitanTraceChannel_Weapon, QueryParams))
	{
		return;
	}

	// The hit actor decides how the damage is applied
	if (ITitanDamageable* Target = Cast<ITitanDamageable>(Hit.GetActor()))
	{
		Target->ReceiveDamage(Damage, Hit);
	}
}

FVector UTitanWeaponComponent::GetMuzzleLocation() const
{
	return GetOwner()->GetActorTransform().TransformPosition(MuzzleOffset);
}

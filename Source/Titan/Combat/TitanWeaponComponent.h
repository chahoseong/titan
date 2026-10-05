#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TitanWeaponComponent.generated.h"

/**
 * Fires shots from the owning actor toward a target point. A shot is a trace that
 * delivers the weapon's damage to the first actor it hits, if that actor is damageable.
 */
UCLASS()
class TITAN_API UTitanWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTitanWeaponComponent();

	/** Fires one shot toward TargetPoint. Does nothing until FireInterval has passed since the last shot */
	void Fire(const FVector& TargetPoint);

	/** World location the shots start from */
	FVector GetMuzzleLocation() const;

	float GetDamage() const { return Damage; }
	float GetFireInterval() const { return FireInterval; }
	float GetRange() const { return Range; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0"))
	float Damage = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0", Units = "s"))
	float FireInterval = 0.1f;

	// Measured from the muzzle location
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0", Units = "cm"))
	float Range = 10000.0f;

	// Where the shots start, relative to the owner. Stands in for a muzzle until there is a weapon mesh
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FVector MuzzleOffset = FVector(30.0f, 15.0f, 50.0f);

	// World time of the last shot. Unset until the first shot
	TOptional<double> LastFireTime;
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TitanAimComponent.generated.h"

class UCameraComponent;
class UCharacterMovementComponent;
class USpringArmComponent;

/**
 * Owns the player's aim state and blends movement speed and camera view
 * between the owner's default values and the aim values.
 */
UCLASS()
class TITAN_API UTitanAimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTitanAimComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartAiming();
	void StopAiming();

	bool IsAiming() const { return bIsAiming; }

private:
	void ApplyBlend(float Alpha) const;

	UPROPERTY(EditDefaultsOnly, Category = "Aim", meta = (ClampMin = "0.0", Units = "cm/s"))
	float AimWalkSpeed = 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim", meta = (ClampMin = "0.0", Units = "cm"))
	float AimArmLength = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	FVector AimShoulderOffset = FVector(0.0f, 70.0f, 60.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Aim", meta = (ClampMin = "5.0", ClampMax = "170.0", Units = "deg"))
	float AimFieldOfView = 70.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim", meta = (ClampMin = "0.0", Units = "s"))
	float AimTransitionTime = 0.2f;

	UPROPERTY(VisibleInstanceOnly, Category = "Aim")
	bool bIsAiming = false;

	// 0 = default view, 1 = aim view
	float AimAlpha = 0.0f;

	TWeakObjectPtr<UCharacterMovementComponent> Movement;
	TWeakObjectPtr<USpringArmComponent> CameraBoom;
	TWeakObjectPtr<UCameraComponent> Camera;

	float DefaultWalkSpeed = 0.0f;
	float DefaultArmLength = 0.0f;
	FVector DefaultShoulderOffset = FVector::ZeroVector;
	float DefaultFieldOfView = 0.0f;
};

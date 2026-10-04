#pragma once

#include "CoreMinimal.h"
#include "Characters/TitanCharacter.h"
#include "TitanPlayerCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UTitanAimComponent;
class UTitanWeaponComponent;
struct FInputActionValue;

UCLASS(Abstract)
class TITAN_API ATitanPlayerCharacter : public ATitanCharacter
{
	GENERATED_BODY()

public:
	ATitanPlayerCharacter();

	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UTitanAimComponent* GetAimComponent() const { return AimComponent; }

private:
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_AimStarted();
	void Input_AimCompleted();
	void Input_Fire();

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "Aim")
	TObjectPtr<UTitanAimComponent> AimComponent;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<UTitanWeaponComponent> WeaponComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;
};

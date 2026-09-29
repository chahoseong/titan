#pragma once

#include "CoreMinimal.h"
#include "Characters/TitanCharacter.h"
#include "TitanPlayerCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
struct FInputActionValue;

UCLASS(Abstract)
class TITAN_API ATitanPlayerCharacter : public ATitanCharacter
{
	GENERATED_BODY()

public:
	ATitanPlayerCharacter();

	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
};

#include "Characters/TitanPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

ATitanPlayerCharacter::ATitanPlayerCharacter()
{
	// Face the camera's horizontal direction regardless of movement direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->MaxWalkSpeed = 400.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 60.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->SetFieldOfView(90.0f);
	FollowCamera->bUsePawnControlRotation = false;
}

void ATitanPlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (ensureMsgf(DefaultMappingContext, TEXT("%s has no DefaultMappingContext"), *GetName()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void ATitanPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
}

void ATitanPlayerCharacter::Input_Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();

	// Use only the camera yaw so looking up or down does not tilt movement
	const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	const FRotationMatrix YawMatrix(YawRotation);

	// CharacterMovement clamps the summed input to length 1, so diagonal speed equals straight speed
	AddMovementInput(YawMatrix.GetUnitAxis(EAxis::X), Input.Y);
	AddMovementInput(YawMatrix.GetUnitAxis(EAxis::Y), Input.X);
}

void ATitanPlayerCharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();

	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}

#include "Combat/TitanAimComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

UTitanAimComponent::UTitanAimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTitanAimComponent::BeginPlay()
{
	Super::BeginPlay();

	const AActor* Owner = GetOwner();
	Movement = Owner->FindComponentByClass<UCharacterMovementComponent>();
	CameraBoom = Owner->FindComponentByClass<USpringArmComponent>();
	Camera = Owner->FindComponentByClass<UCameraComponent>();

	if (!ensureMsgf(Movement.IsValid() && CameraBoom.IsValid() && Camera.IsValid(),
		TEXT("%s needs CharacterMovement, SpringArm and Camera components on %s"), *GetName(), *Owner->GetName()))
	{
		SetComponentTickEnabled(false);
		return;
	}

	// The owner's component values authored in the editor are the default view
	DefaultWalkSpeed = Movement->MaxWalkSpeed;
	DefaultArmLength = CameraBoom->TargetArmLength;
	DefaultShoulderOffset = CameraBoom->SocketOffset;
	DefaultFieldOfView = Camera->FieldOfView;
}

void UTitanAimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float TargetAlpha = bIsAiming ? 1.0f : 0.0f;
	if (AimAlpha == TargetAlpha)
	{
		return;
	}

	// Move from the current alpha so reversing mid-transition never jumps
	AimAlpha = AimTransitionTime > 0.0f
		? FMath::FInterpConstantTo(AimAlpha, TargetAlpha, DeltaTime, 1.0f / AimTransitionTime)
		: TargetAlpha;

	ApplyBlend(FMath::SmoothStep(0.0f, 1.0f, AimAlpha));
}

void UTitanAimComponent::StartAiming()
{
	bIsAiming = true;
}

void UTitanAimComponent::StopAiming()
{
	bIsAiming = false;
}

void UTitanAimComponent::ApplyBlend(float Alpha) const
{
	Movement->MaxWalkSpeed = FMath::Lerp(DefaultWalkSpeed, AimWalkSpeed, Alpha);
	CameraBoom->TargetArmLength = FMath::Lerp(DefaultArmLength, AimArmLength, Alpha);
	CameraBoom->SocketOffset = FMath::Lerp(DefaultShoulderOffset, AimShoulderOffset, Alpha);
	Camera->SetFieldOfView(FMath::Lerp(DefaultFieldOfView, AimFieldOfView, Alpha));
}

#include "Combat/TitanAimComponent.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "TitanCollisionChannels.h"

static TAutoConsoleVariable<bool> CVarDrawAimPoint(
	TEXT("Titan.Debug.DrawAimPoint"),
	false,
	TEXT("Draw the player's aim point."));

UTitanAimComponent::UTitanAimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Run after the camera manager updates so the aim point uses this frame's view
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
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

	UpdateAimBlend(DeltaTime);
	UpdateAimPoint();
}

void UTitanAimComponent::UpdateAimBlend(float DeltaTime)
{
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

void UTitanAimComponent::UpdateAimPoint()
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	const APlayerController* PlayerController = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return;
	}

	// The screen center lies on the forward axis of the final rendered view
	const FMinimalViewInfo& View = PlayerController->PlayerCameraManager->GetCameraCacheView();
	const FVector TraceStart = View.Location;
	const FVector TraceEnd = TraceStart + View.Rotation.Vector() * AimTraceMaxDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TitanAimPoint));
	QueryParams.AddIgnoredActor(OwnerPawn);

	FHitResult Hit;
	AimPoint = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TitanTraceChannel_Weapon, QueryParams)
		? Hit.ImpactPoint
		: TraceEnd;

#if ENABLE_DRAW_DEBUG
	if (CVarDrawAimPoint.GetValueOnGameThread())
	{
		DrawDebugPoint(GetWorld(), AimPoint, 8.0f, FColor::Red, false, -1.0f, SDPG_Foreground);
	}
#endif
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

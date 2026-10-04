#include "UI/TitanHUD.h"

#include "Blueprint/UserWidget.h"
#include "Combat/TitanHealthComponent.h"
#include "EngineUtils.h"

static TAutoConsoleVariable<bool> CVarShowHealth(
	TEXT("Titan.Debug.ShowHealth"),
	false,
	TEXT("Show the health of every actor that has a health component."));

void ATitanHUD::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(CrosshairWidgetClass, TEXT("%s has no CrosshairWidgetClass"), *GetName()))
	{
		if (UUserWidget* Crosshair = CreateWidget<UUserWidget>(GetOwningPlayerController(), CrosshairWidgetClass))
		{
			Crosshair->AddToViewport();
		}
	}
}

void ATitanHUD::DrawHUD()
{
	Super::DrawHUD();

#if ENABLE_DRAW_DEBUG
	if (CVarShowHealth.GetValueOnGameThread())
	{
		const float TextScale = 1.5f;
		const float LineHeight = 24.0f;
		float ScreenY = 60.0f;

		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			const UTitanHealthComponent* Health = It->FindComponentByClass<UTitanHealthComponent>();
			if (!Health)
			{
				continue;
			}

			const FString Text = Health->IsDead()
				? FString::Printf(TEXT("%s: Dead"), *It->GetActorNameOrLabel())
				: FString::Printf(TEXT("%s: Health %.0f / %.0f"), *It->GetActorNameOrLabel(), Health->GetHealth(), Health->GetMaxHealth());

			DrawText(Text, FLinearColor::White, 60.0f, ScreenY, nullptr, TextScale);
			ScreenY += LineHeight;
		}
	}
#endif
}

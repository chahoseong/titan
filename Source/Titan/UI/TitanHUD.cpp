#include "UI/TitanHUD.h"

#include "Blueprint/UserWidget.h"

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

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TitanHUD.generated.h"

class UUserWidget;

UCLASS(Abstract)
class TITAN_API ATitanHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;
};

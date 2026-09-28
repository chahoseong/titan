#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TitanGameMode.generated.h"

UCLASS(Abstract)
class TITAN_API ATitanGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATitanGameMode();
};

#include "Game/TitanGameMode.h"

#include "Characters/TitanPlayerCharacter.h"

ATitanGameMode::ATitanGameMode()
{
	DefaultPawnClass = ATitanPlayerCharacter::StaticClass();
}

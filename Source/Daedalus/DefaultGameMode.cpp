#include "Daedalus.h"
#include "DefaultGameMode.h"
#include "PlayerCharacter.h"


ADefaultGameMode::ADefaultGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
}



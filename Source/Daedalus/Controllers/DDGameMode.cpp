#include "Daedalus.h"
#include "DDGameMode.h"
#include "PlayerCharacter.h"
#include "DDGameState.h"


ADDGameMode::ADDGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	GameStateClass = ADDGameState::StaticClass();
}

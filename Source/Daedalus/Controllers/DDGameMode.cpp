#include "Daedalus.h"
#include "DDGameMode.h"
#include "PlayerCharacter.h"
#include "DebugCharacter.h"
#include "DDGameState.h"


ADDGameMode::ADDGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	DefaultPawnClass = ADebugCharacter::StaticClass();
	GameStateClass = ADDGameState::StaticClass();
}

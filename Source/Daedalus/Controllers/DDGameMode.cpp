#include "Daedalus.h"
#include "DDGameMode.h"
#include <Actors/Characters/PlayerCharacter.h>
#include <Actors/Characters/DebugCharacter.h>
#include <Controllers/DDGameState.h>
#include <Controllers/DDPlayerController.h>


ADDGameMode::ADDGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	DefaultPawnClass = ADebugCharacter::StaticClass();
	GameStateClass = ADDGameState::StaticClass();
	PlayerControllerClass = ADDPlayerController::StaticClass();
}

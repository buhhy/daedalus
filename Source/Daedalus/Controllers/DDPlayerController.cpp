#include <Daedalus.h>
#include "DDPlayerController.h"

#include <Actors/Characters/PlayerCharacter.h>
#include <Controllers/DDGameState.h>
#include <Utilities/UnrealBridge.h>

using namespace events;
using namespace utils;

ADDPlayerController::ADDPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

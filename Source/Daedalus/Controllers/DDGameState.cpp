#include "Daedalus.h"
#include "DDGameState.h"

ADDGameState::ADDGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	EventBus = NewObject<UEventBus>();
}

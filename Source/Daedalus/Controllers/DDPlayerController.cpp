#include <Daedalus.h>
#include "DDPlayerController.h"

#include <Actors/Characters/PlayerCharacter.h>
#include <Controllers/DDGameState.h>
#include <Utilities/UnrealBridge.h>

using namespace events;
using namespace utils;

ADDPlayerController::ADDPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), PositionSecondCount(0), ViewSecondCount(0)
{

}

void ADDPlayerController::Tick(float delta) {
	Super::Tick(delta);
	PositionSecondCount += delta;
	ViewSecondCount += delta;

	auto pawn = this->GetPawn();

	if (pawn != NULL) {
		// Tick once every 20 milliseconds
		if (ViewSecondCount >= 0.02) {
			ViewSecondCount -= 0.02;

			FVector pos;
			FRotator dir;
			GetPlayerViewPoint(pos, dir);
			GetWorld()->GetGameState<ADDGameState>()->EventBus->BroadcastEvent(
				EventDataPtr(new EViewPosition(ToVector3D(pos), ToVector3D(dir.Vector()))));
		}

		// Tick once every half-second
		if (PositionSecondCount >= 0.5) {
			PositionSecondCount -= 0.5;
		
			GetWorld()->GetGameState<ADDGameState>()->EventBus->BroadcastEvent(
				EventDataPtr(new EPlayerPosition(ToVector3D(pawn->GetActorLocation()))));
		}
	}
}

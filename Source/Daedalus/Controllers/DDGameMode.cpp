#include "Daedalus.h"
#include "DDGameMode.h"
#include <Actors/Characters/PlayerCharacter.h>
#include <Actors/Characters/DebugCharacter.h>
#include <Actors/SlateUI/PlayerHUD.h>
#include <Controllers/DDGameState.h>
#include <Controllers/DDPlayerController.h>

ADDGameMode::ADDGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> playerPawnObject(
		TEXT("Blueprint'/Game/PlayerCharacter/Male/BlueprintPlayer.BlueprintPlayer'"));
	DefaultPawnClass = (UClass *) playerPawnObject.Object->GeneratedClass;
	GameStateClass = ADDGameState::StaticClass();
	PlayerControllerClass = ADDPlayerController::StaticClass();
	HUDClass = APlayerHUD::StaticClass();
}

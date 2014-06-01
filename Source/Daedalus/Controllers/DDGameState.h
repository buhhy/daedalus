#pragma once

#include "GameFramework/GameState.h"
#include "EventBus/EventBus.h"
#include "DDGameState.generated.h"

/**
 * 
 */
UCLASS()
class ADDGameState : public AGameState
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Game)
		UEventBus * EventBus;
};

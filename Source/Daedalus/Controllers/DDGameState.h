#pragma once

#include "GameFramework/GameState.h"
#include "EventBus/EventBus.h"
#include "ChunkLoader.h"
#include "TerrainDataStructures.h"
#include "DDGameState.generated.h"

/**
 * 
 */
UCLASS()
class ADDGameState : public AGameState
{
	GENERATED_UCLASS_BODY()
private:
	terrain::TerrainGeneratorParameters TerrainGenParams;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Game)
		UEventBus * EventBus;

	TSharedRef<terrain::ChunkLoader> ChunkLoader;
};

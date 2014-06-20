#pragma once

#include "GameFramework/GameState.h"
#include "EventBus.h"
#include "ChunkLoader.h"
#include "BiomeRegionLoader.h"
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
	int64 Seed;
	terrain::TerrainGeneratorParameters TerrainGenParams;
	terrain::BiomeGeneratorParameters BiomeGenParams;

public:
	TSharedRef<events::EventBus> EventBus;
	TSharedRef<terrain::ChunkLoader> ChunkLoader;
	TSharedRef<terrain::BiomeRegionLoader> BiomeRegionLoader;
};

#pragma once

#include "GameFramework/GameState.h"
#include "EventBus.h"
#include "ChunkLoader.h"
#include "BiomeRegionLoader.h"
#include "TerrainDataStructures.h"

#include <memory>

#include "DDGameState.generated.h"

/**
 * 
 */
UCLASS()
class ADDGameState : public AGameState {
	GENERATED_UCLASS_BODY()

private:
	const Int64 Seed;
	const terrain::TerrainGeneratorParameters TerrainGenParams;
	const terrain::BiomeGeneratorParameters BiomeGenParams;

public:
	events::EventBusPtr EventBus;
	terrain::ChunkLoaderPtr ChunkLoader;
	std::shared_ptr<terrain::BiomeRegionLoader> BiomeRegionLoader;
};

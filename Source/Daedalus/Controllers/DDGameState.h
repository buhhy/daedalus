#pragma once

#include "GameFramework/GameState.h"

#include <Controllers/EventBus/EventBus.h>
#include <Models/Terrain/ChunkLoader.h>
#include <Models/Terrain/BiomeRegionLoader.h>
#include <Models/Terrain/TerrainDataStructures.h>

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

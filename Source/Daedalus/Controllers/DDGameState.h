#pragma once

#include <GameFramework/GameState.h>

#include <Controllers/ResourceCache.h>
#include <Controllers/EventBus/EventBus.h>
#include <Models/Terrain/ChunkLoader.h>
#include <Models/Terrain/BiomeRegionLoader.h>
#include <Models/Terrain/TerrainDataStructures.h>

#include <memory>

#include "DDGameState.generated.h"

namespace items {
	class ItemDataFactory;
	using ItemDataFactoryPtr = std::shared_ptr<ItemDataFactory>;
	using ItemDataFactoryCPtr = std::shared_ptr<const ItemDataFactory>;
}

class ResourceCache;
using ResourceCachePtr = std::shared_ptr<ResourceCache>;

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
	
	items::ItemDataFactoryPtr itemDataFactoryRef;
	ResourceCachePtr resourceCacheRef;

public:
	events::EventBusPtr EventBus;
	terrain::ChunkLoaderPtr ChunkLoader;
	terrain::BiomeRegionLoaderPtr BiomeRegionLoader;

	items::ItemDataFactoryCPtr getItemDataFactoryRef() const;
	ResourceCacheCPtr getResourceCacheRef() const;
};

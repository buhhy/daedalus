#pragma once

#include <Actors/Terrain/Chunk.h>
#include <Controllers/DDGameState.h>
#include <Controllers/EventBus/EventBus.h>
#include <Models/Items/ItemDataFactory.h>
#include <Models/Terrain/TerrainDataStructures.h>

#include <unordered_map>
#include <memory>

#include "ChunkManager.generated.h"

using events::EventListener;

/**
 * This class takes data fetched from the ChunkLoader and renders it. It is
 * responsible for knowing when to load and dispose of rendered chunks. This
 * will most likely run on the client-side.
 */
UCLASS()
class AChunkManager : public AActor, public EventListener {
	GENERATED_UCLASS_BODY()

private:
	using ChunkCache = std::unordered_map<terrain::ChunkOffsetVector, AChunk *>;

	ChunkCache LocalCache;

	const Uint64 RenderDistance;                      // Specified in number of chunks

	const terrain::TerrainGeneratorParameters * GenParams;
	events::EventBusPtr EventBusRef;
	terrain::ChunkLoaderPtr ChunkLoaderRef;

	

	inline ADDGameState * GetGameState() { return GetWorld()->GetGameState<ADDGameState>(); }
	AChunk * GetChunkAt(const terrain::ChunkOffsetVector & point);
	void UpdateChunksAt(const utils::Vector3D<> & playerPosition);

public:
	virtual void HandleEvent(const events::EventDataPtr & data) override;
	virtual void BeginPlay() override;

	utils::Option<terrain::TerrainRaytraceResult> Raytrace(
		const utils::Ray3D & viewpoint, const double maxDist);
	/**
	 * Creates a new item actor from the given item data. The item data is not duplicated, so
	 * be sure to clone the item data before passing it in if spawning a brand new item.
	 */
	AItem * PlaceItem(const items::ItemDataPtr & data);
};

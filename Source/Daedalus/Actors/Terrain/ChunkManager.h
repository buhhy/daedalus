#pragma once

#include <Actors/Items/ItemFactory.h>
#include <Actors/Terrain/Chunk.h>
#include <Controllers/DDGameState.h>
#include <Controllers/EventBus/EventBus.h>
#include <Models/Terrain/TerrainDataStructures.h>

#include <unordered_map>
#include <memory>

#include "ChunkManager.generated.h"


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
	Uint64 RenderDistance;

	const UItemFactory * ItemFactory;

	

	inline ADDGameState * GetGameState() { return GetWorld()->GetGameState<ADDGameState>(); }
	void UpdateChunksAt(const utils::Vector3D<> & playerPosition);

public:virtual void HandleEvent(
		const events::EventType type,
		const std::shared_ptr<events::EventData> & data) override;

	virtual void BeginPlay() override;
};

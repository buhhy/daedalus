#pragma once

#include <Actors/Terrain/BiomeRegion.h>
#include <Controllers/DDGameState.h>
#include <Controllers/EventBus/EventBus.h>
#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra2D.h>

#include <unordered_map>
#include <memory>

#include "BiomeRegionManager.generated.h"

using events::EventListener;

/**
 * This manages biome rendering for debugging purposes.
 */
UCLASS()
class ABiomeRegionManager : public AActor, public EventListener {
	GENERATED_UCLASS_BODY()

private:
	typedef std::unordered_map <
		terrain::BiomeRegionOffsetVector, ABiomeRegion *
	> BiomeRegionCache;

	BiomeRegionCache LocalCache;
	uint64 RenderDistance;
	float RenderHeight;

	ADDGameState * GetGameState();
	void UpdateBiomesAt(const utils::Vector3D<> & playerPosition);
	void ReloadRegionAt(const terrain::BiomeRegionOffsetVector & offset);
	bool DeleteRegionAt(const terrain::BiomeRegionOffsetVector & offset);

public:
	virtual void HandleEvent(const events::EventDataPtr & data) override;
	virtual void BeginPlay() override;
};

#include "Daedalus.h"
#include "DDGameState.h"

// TODO: terrain generation data should probably be loaded from a settings file.
ADDGameState::ADDGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP),
	Seed(12345678),
	TerrainGenParams({ { 16, 16, 16 }, Seed, 40.0 }),
	BiomeGenParams({
		{ 16, 16 },      // BiomeGridCellSize
		Seed,            // Seed
		3,               // BufferSize
		1,               // MinPointsPerCell
		1,               // MaxPointsPerCell
		10000.0          // BiomeScale
	}),
	ChunkLoader(new terrain::ChunkLoader(TerrainGenParams)),
	BiomeRegionLoader(new terrain::BiomeRegionLoader(BiomeGenParams))
{
	EventBus = NewObject<UEventBus>();
}

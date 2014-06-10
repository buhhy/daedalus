#include "Daedalus.h"
#include "DDGameState.h"

ADDGameState::ADDGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP),
	Seed(12345678),
	TerrainGenParams({ { 16, 16, 16 }, Seed, 40.0 }),
	BiomeGenParams({ { 32, 32 }, Seed, 100.0 }),
	ChunkLoader(new terrain::ChunkLoader(TerrainGenParams)),
	BiomeRegionLoader(new terrain::BiomeRegionLoader(BiomeGenParams))
{
	EventBus = NewObject<UEventBus>();
}

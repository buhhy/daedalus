#include "Daedalus.h"
#include "DDGameState.h"

// TODO: terrain generation data should probably be loaded from a settings file.
ADDGameState::ADDGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP),
	Seed(12345678),
	TerrainGenParams({
		16,              // Number of grid cells along a single axis
		Seed,            // Seed
		512.0            // Size of the chunk in real units along a single axis
	}),
	BiomeGenParams({
		16,              // Number of grid cells along a single axis
		Seed,            // Seed
		4,               // Number of buffer cells in grid
		1,               // Minimum bound of number of points
		1,               // Maximum bound of number of points
		16 * 0x10      // Size of the biome region in real units along a single axis
	}),
	EventBus(new events::EventBus()),
	ChunkLoader(new terrain::ChunkLoader(TerrainGenParams)),
	BiomeRegionLoader(new terrain::BiomeRegionLoader(BiomeGenParams, EventBus))
{}

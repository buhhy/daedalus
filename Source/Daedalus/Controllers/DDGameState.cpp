#include "Daedalus.h"
#include "DDGameState.h"

// TODO: terrain generation data should probably be loaded from a settings file.
/*
 Real units are specified in centimetres. Each ingame grid cell is 50cm * 50cm * 50cm.
 */
ADDGameState::ADDGameState(const class FPostConstructInitializeProperties& PCIP) :
	Super(PCIP),
	Seed(12345678),
	TerrainGenParams({
		16,              // Number of grid cells along a single axis
		Seed,            // Seed
		16 * 50          // Size of the chunk in real units along a single axis
	}),
	BiomeGenParams({
		32,              // Number of grid cells along a single axis
		Seed,            // Seed
		4,               // Number of buffer cells in grid
		1,               // Minimum bound of number of points
		1,               // Maximum bound of number of points
		64 * 0x1000      // Size of the biome region in real units along a single axis
	}),
	EventBus(new events::EventBus())
{
	BiomeRegionLoader = std::shared_ptr<terrain::BiomeRegionLoader>(
		new terrain::BiomeRegionLoader(BiomeGenParams, EventBus));
	ChunkLoader = std::shared_ptr<terrain::ChunkLoader>(
		new terrain::ChunkLoader(TerrainGenParams, BiomeRegionLoader));
}

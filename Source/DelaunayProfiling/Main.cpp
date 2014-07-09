#include <cstdio>
#include <Models/Terrain/BiomeRegionLoader.h>
#include <Utilities/Graph/Delaunay.h>
#include <Controllers/EventBus/EventBus.h>

using namespace utils;
using namespace delaunay;
using namespace terrain;

#define ITERATOR_DEBUG_LEVEL 0

void Run() {
	Int64 Seed = 12345678;
	std::shared_ptr<DelaunayBuilderDAC2D> Builder(new DelaunayBuilderDAC2D(0, NULL));
	std::shared_ptr<events::EventBus> MockBus(new events::EventBus());
	BiomeGeneratorParameters GenParams = {
		64,              // Number of grid cells along a single axis
		Seed,            // Seed
		4,               // Number of buffer cells in grid
		1,               // Minimum bound of number of points
		1,               // Maximum bound of number of points
		16 * 0x10        // Size of the biome region in real units along a single axis
	};
	BiomeRegionLoader RegionLoader(
		GenParams,MockBus, BiomeRegionLoader::DelaunayBuilderPtr(Builder), 1);
	RegionLoader.GetBiomeRegionAt({ 0, 0 });

	Uint32 width = 800, height = 800;
	
	for (Uint32 y = 0; y < height; y++) {
		for (Uint32 x = 0; x < width; x++) {
			double dx = GenParams.BiomeScale * x / width;
			double dy = GenParams.BiomeScale * y / height;
			auto id = RegionLoader.FindNearestBiomeId({ dx, dy });
			auto data = RegionLoader.GetBiomeAt(id);
		}
	}
}

int main(int argv, char ** argc) {
	Run();
	return 0;
}

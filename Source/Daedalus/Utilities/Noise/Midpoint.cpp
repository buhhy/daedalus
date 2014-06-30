#include <Daedalus.h>
#include "Midpoint.h"

namespace utils {
	//void ChunkLoader::RunDiamondSquare(ChunkData & data) {
	//	uint32_t w = DensityData.Width;
	//	uint32_t h = DensityData.Height;
	//	uint32_t d = DensityData.Depth;
	//	float ** heightMap = new float *[w];

	//	// Initialization
	//	for (uint32_t x = 0; x < w; x++) {
	//		heightMap[x] = new float[d];

	//		for (uint32_t z = 0; z < d; z++)
	//			heightMap[x][z] = 0.0;
	//	}

	//	// Diamond square algorithm
	//	float perturb = 1.0;
	//	for (uint32_t interval = w; interval > 1; interval /= 2) {
	//		uint32_t halfInterval = interval / 2;

	//		// Diamond step
	//		for (uint32_t x = halfInterval; x < w; x += interval) {
	//			for (uint32_t y = halfInterval; y < d; y += interval) {
	//				auto topl = heightMap[x - halfInterval][y - halfInterval];
	//				auto topr = heightMap[x + halfInterval][y - halfInterval];
	//				auto botl = heightMap[x - halfInterval][y + halfInterval];
	//				auto botr = heightMap[x + halfInterval][y + halfInterval];
	//				auto avg = (topl + topr + botl + botr) / 4;

	//				auto rand = (utils::hashFromVector(Seed, ChunkOffset * ChunkSize + utils::Vector3<int64_t>(x, y, FMath::Round(avg))) * 2.0 - 1.0) * perturb;
	//				heightMap[x][y] = avg + rand;
	//			}
	//		}

	//		// Square step
	//		for (uint32_t x = 0, i = 0; x < w; x += interval / 2, i++) {
	//			for (uint32_t y = 0, j = 0; y < d; y += interval / 2, j++) {
	//				if ((i % 2 != 0) != (j % 2 != 0)) {
	//					auto top = y == 0 ? 0.0 : heightMap[x][y - halfInterval];
	//					auto bottom = y == d - 1 ? 0.0 : heightMap[x][y + halfInterval];
	//					auto left = x == 0 ? 0.0 : heightMap[x - halfInterval][y];
	//					auto right = x == w - 1 ? 0.0 : heightMap[x + halfInterval][y];
	//					auto avg = (top + bottom + left + right) / 4;

	//					auto rand = (utils::hashFromVector(Seed, ChunkOffset * ChunkSize + utils::Vector3<int64_t>(x, y, FMath::Round(avg))) * 2.0 - 1.0) * perturb;
	//					heightMap[x][y] = avg + rand;
	//				}
	//			}
	//		}

	//		perturb /= 2;
	//	}

	//	for (uint32_t x = 0; x < w; x++) {
	//		for (uint32_t y = 0; y < d; y++) {
	//			float height = heightMap[x][y] * 5.5 + h / 2;
	//			height = FMath::Round(height * 2.0) / 2.0; // Round to nearest .5
	//			for (uint64_t z = 0; height > 0; height -= 1, z++)
	//				DensityData.Set(x, y, z, 1.0);
	//		}
	//	}
	//}
}

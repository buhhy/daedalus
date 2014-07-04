#include <Daedalus.h>
#include "Midpoint.h"

namespace utils {
	//void ChunkLoader::RunDiamondSquare(ChunkData & data) {
	//	Uint32 w = DensityData.Width;
	//	Uint32 h = DensityData.Height;
	//	Uint32 d = DensityData.Depth;
	//	float ** heightMap = new float *[w];

	//	// Initialization
	//	for (Uint32 x = 0; x < w; x++) {
	//		heightMap[x] = new float[d];

	//		for (Uint32 z = 0; z < d; z++)
	//			heightMap[x][z] = 0.0;
	//	}

	//	// Diamond square algorithm
	//	float perturb = 1.0;
	//	for (Uint32 interval = w; interval > 1; interval /= 2) {
	//		Uint32 halfInterval = interval / 2;

	//		// Diamond step
	//		for (Uint32 x = halfInterval; x < w; x += interval) {
	//			for (Uint32 y = halfInterval; y < d; y += interval) {
	//				auto topl = heightMap[x - halfInterval][y - halfInterval];
	//				auto topr = heightMap[x + halfInterval][y - halfInterval];
	//				auto botl = heightMap[x - halfInterval][y + halfInterval];
	//				auto botr = heightMap[x + halfInterval][y + halfInterval];
	//				auto avg = (topl + topr + botl + botr) / 4;

	//				auto rand = (utils::hashFromVector(Seed, ChunkOffset * ChunkSize + utils::Vector3<Int64>(x, y, FMath::Round(avg))) * 2.0 - 1.0) * perturb;
	//				heightMap[x][y] = avg + rand;
	//			}
	//		}

	//		// Square step
	//		for (Uint32 x = 0, i = 0; x < w; x += interval / 2, i++) {
	//			for (Uint32 y = 0, j = 0; y < d; y += interval / 2, j++) {
	//				if ((i % 2 != 0) != (j % 2 != 0)) {
	//					auto top = y == 0 ? 0.0 : heightMap[x][y - halfInterval];
	//					auto bottom = y == d - 1 ? 0.0 : heightMap[x][y + halfInterval];
	//					auto left = x == 0 ? 0.0 : heightMap[x - halfInterval][y];
	//					auto right = x == w - 1 ? 0.0 : heightMap[x + halfInterval][y];
	//					auto avg = (top + bottom + left + right) / 4;

	//					auto rand = (utils::hashFromVector(Seed, ChunkOffset * ChunkSize + utils::Vector3<Int64>(x, y, FMath::Round(avg))) * 2.0 - 1.0) * perturb;
	//					heightMap[x][y] = avg + rand;
	//				}
	//			}
	//		}

	//		perturb /= 2;
	//	}

	//	for (Uint32 x = 0; x < w; x++) {
	//		for (Uint32 y = 0; y < d; y++) {
	//			float height = heightMap[x][y] * 5.5 + h / 2;
	//			height = FMath::Round(height * 2.0) / 2.0; // Round to nearest .5
	//			for (Uint64 z = 0; height > 0; height -= 1, z++)
	//				DensityData.Set(x, y, z, 1.0);
	//		}
	//	}
	//}
}

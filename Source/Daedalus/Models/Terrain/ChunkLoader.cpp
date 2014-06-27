#include "Daedalus.h"
#include "ChunkLoader.h"

namespace terrain {
	ChunkLoader::ChunkLoader(const TerrainGeneratorParameters & params) {
		this->TerrainGenParams = params;
	}

	ChunkLoader::~ChunkLoader() {
		LoadedChunkCache.empty();
	}

	std::shared_ptr<ChunkData> ChunkLoader::LoadChunkFromDisk(
		const utils::Vector3<int64_t> & offset
	) {
		return std::shared_ptr<ChunkData>(NULL);
	}

	std::shared_ptr<ChunkData> ChunkLoader::GenerateMissingChunk(
		const utils::Vector3<int64_t> & offset
	) {
		auto data = new ChunkData(TerrainGenParams.GridCellCount, offset);
		SetDefaultHeight(*data, 32.0);
		return std::shared_ptr<ChunkData>(data);
	}

	std::shared_ptr<ChunkData> ChunkLoader::GetChunkAt(
		const utils::Vector3<int64_t> & offset
	) {
		//UE_LOG(LogTemp, Error, TEXT("Loading chunk at offset: %d %d %d"), offset.X, offset.Y, offset.Z);
		if (LoadedChunkCache.count(offset) > 0) {
			return LoadedChunkCache.at(offset);
		} else {
			auto loaded = LoadChunkFromDisk(offset);
			return loaded ? loaded : GenerateMissingChunk(offset);
		}
	}

	const TerrainGeneratorParameters & ChunkLoader::GetGeneratorParameters() const {
		return TerrainGenParams;
	}

	void ChunkLoader::SetDefaultHeight(ChunkData & data, int32_t height) {
		// TODO: if the chunk height ended on a chunk division line, no triangles are generated
		auto chunkHeight = TerrainGenParams.ChunkScale;
		if (((data.ChunkOffset.Z + 1) * (int64_t) chunkHeight) < height) {
			data.DensityData.Fill(1.0);			// Completely filled block
			//UE_LOG(LogTemp, Error, TEXT("Ground chunk"));
		} else if ((data.ChunkOffset.Z * (int64_t) chunkHeight) > height) {
			data.DensityData.Fill(0.0);			// Completely empty block
			//UE_LOG(LogTemp, Error, TEXT("Air chunk"));
		} else {
			//UE_LOG(LogTemp, Error, TEXT("Mixed chunk"));
			auto localHeight = TerrainGenParams.GridCellCount * (height / chunkHeight - data.ChunkOffset.Z);
			for (uint32_t x = 0; x < data.ChunkFieldSize; x++) {
				for (uint32_t y = 0; y < data.ChunkFieldSize; y++) {
					for (uint32_t z = 0; z < data.ChunkFieldSize && z < localHeight; z++)
						data.DensityData.Set(x, y, z, 1);
				}
			}
		}
	}

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

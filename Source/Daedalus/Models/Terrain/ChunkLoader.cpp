#include "Daedalus.h"
#include "ChunkLoader.h"

namespace terrain {
	ChunkLoader::ChunkLoader(const TerrainGeneratorParameters & params) {
		this->TerrainGenParams = params;
	}

	ChunkLoader::~ChunkLoader() {
		LoadedChunkCache.empty();
	}

	TSharedPtr<ChunkData> ChunkLoader::LoadChunkFromDisk(
		const utils::Vector3<int64> & offset
	) {
		return TSharedPtr<ChunkData>(NULL);
	}

	TSharedRef<ChunkData> ChunkLoader::GenerateMissingChunk(
		const utils::Vector3<int64> & offset
	) {
		auto data = new ChunkData(TerrainGenParams.ChunkPolygonSize, offset);
		SetDefaultHeight(*data, 5);
		return TSharedRef<ChunkData>(data);
	}

	TSharedRef<ChunkData> ChunkLoader::GetChunkAt(
		const utils::Vector3<int64> & offset
	) {
		//UE_LOG(LogTemp, Error, TEXT("Loading chunk at offset: %d %d %d"), offset.X, offset.Y, offset.Z);
		if (LoadedChunkCache.count(offset) > 0) {
			return LoadedChunkCache.at(offset);
		} else {
			auto loaded = LoadChunkFromDisk(offset);
			if (loaded.IsValid())
				return loaded.ToSharedRef();
			return GenerateMissingChunk(offset);
		}
	}

	const TerrainGeneratorParameters & ChunkLoader::GetGeneratorParameters() const {
		return TerrainGenParams;
	}

	void ChunkLoader::SetDefaultHeight(ChunkData & data, int32 height) {
		auto chunkHeight = TerrainGenParams.ChunkPolygonSize.Z;
		if (((data.ChunkOffset.Z + 1) * (int64) chunkHeight) < height) {
			data.DensityData.Fill(1.0);			// Completely filled block
			//UE_LOG(LogTemp, Error, TEXT("Ground chunk"));
		} else if ((data.ChunkOffset.Z * (int64) chunkHeight) > height) {
			data.DensityData.Fill(0.0);			// Completely empty block
			//UE_LOG(LogTemp, Error, TEXT("Air chunk"));
		} else {
			//UE_LOG(LogTemp, Error, TEXT("Mixed chunk"));
			auto localHeight = height - chunkHeight * data.ChunkOffset.Z;
			for (uint32 x = 0; x < data.ChunkFieldSize.X; x++) {
				for (uint32 y = 0; y < data.ChunkFieldSize.Y; y++) {
					for (uint32 z = 0; z < data.ChunkFieldSize.Z && z < localHeight; z++)
						data.DensityData.Set(x, y, z, 1);
				}
			}
		}
	}

	//void ChunkLoader::RunDiamondSquare(ChunkData & data) {
	//	uint32 w = DensityData.Width;
	//	uint32 h = DensityData.Height;
	//	uint32 d = DensityData.Depth;
	//	float ** heightMap = new float *[w];

	//	// Initialization
	//	for (uint32 x = 0; x < w; x++) {
	//		heightMap[x] = new float[d];

	//		for (uint32 z = 0; z < d; z++)
	//			heightMap[x][z] = 0.0;
	//	}

	//	// Diamond square algorithm
	//	float perturb = 1.0;
	//	for (uint32 interval = w; interval > 1; interval /= 2) {
	//		uint32 halfInterval = interval / 2;

	//		// Diamond step
	//		for (uint32 x = halfInterval; x < w; x += interval) {
	//			for (uint32 y = halfInterval; y < d; y += interval) {
	//				auto topl = heightMap[x - halfInterval][y - halfInterval];
	//				auto topr = heightMap[x + halfInterval][y - halfInterval];
	//				auto botl = heightMap[x - halfInterval][y + halfInterval];
	//				auto botr = heightMap[x + halfInterval][y + halfInterval];
	//				auto avg = (topl + topr + botl + botr) / 4;

	//				auto rand = (utils::hashFromVector(Seed, ChunkOffset * ChunkSize + utils::Vector3<int64>(x, y, FMath::Round(avg))) * 2.0 - 1.0) * perturb;
	//				heightMap[x][y] = avg + rand;
	//			}
	//		}

	//		// Square step
	//		for (uint32 x = 0, i = 0; x < w; x += interval / 2, i++) {
	//			for (uint32 y = 0, j = 0; y < d; y += interval / 2, j++) {
	//				if ((i % 2 != 0) != (j % 2 != 0)) {
	//					auto top = y == 0 ? 0.0 : heightMap[x][y - halfInterval];
	//					auto bottom = y == d - 1 ? 0.0 : heightMap[x][y + halfInterval];
	//					auto left = x == 0 ? 0.0 : heightMap[x - halfInterval][y];
	//					auto right = x == w - 1 ? 0.0 : heightMap[x + halfInterval][y];
	//					auto avg = (top + bottom + left + right) / 4;

	//					auto rand = (utils::hashFromVector(Seed, ChunkOffset * ChunkSize + utils::Vector3<int64>(x, y, FMath::Round(avg))) * 2.0 - 1.0) * perturb;
	//					heightMap[x][y] = avg + rand;
	//				}
	//			}
	//		}

	//		perturb /= 2;
	//	}

	//	for (uint32 x = 0; x < w; x++) {
	//		for (uint32 y = 0; y < d; y++) {
	//			float height = heightMap[x][y] * 5.5 + h / 2;
	//			height = FMath::Round(height * 2.0) / 2.0; // Round to nearest .5
	//			for (uint64 z = 0; height > 0; height -= 1, z++)
	//				DensityData.Set(x, y, z, 1.0);
	//		}
	//	}
	//}
}

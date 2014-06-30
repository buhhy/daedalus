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
		SetDefaultHeight(*data, 32);
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
}

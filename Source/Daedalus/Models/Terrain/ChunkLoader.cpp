#include "Daedalus.h"
#include "ChunkLoader.h"

namespace terrain {
	ChunkLoader::ChunkLoader() {}

	ChunkLoader::~ChunkLoader() {
		LoadedChunkCache.empty();
	}

	TSharedPtr<ChunkData> ChunkLoader::LoadChunkFromDisk(
		const utils::Vector3<int64> & offset
	) {
		return TSharedRef<ChunkData>(new ChunkData());
	}

	TSharedRef<ChunkData> ChunkLoader::GenerateMissingChunk(
		const utils::Vector3<int64> & offset
	) {
		return TSharedRef<ChunkData>(new ChunkData());
	}

	void ChunkLoader::InitializeChunkLoader(
		const utils::Vector3<uint64> & chunkSize,
		uint64 seed
	) {

	}

	TSharedRef<ChunkData> ChunkLoader::GetChunkAt(
		const utils::Vector3<int64> & offset
	) {
		if (LoadedChunkCache.count(offset) > 0) {
			return LoadedChunkCache.at(offset);
		} else {
			auto loaded = LoadChunkFromDisk(offset);
			if (loaded.IsValid())
				return loaded.ToSharedRef();
			return GenerateMissingChunk(offset);
		}
	}
}

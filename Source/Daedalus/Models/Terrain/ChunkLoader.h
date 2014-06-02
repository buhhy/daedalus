#pragma once

#include <unordered_map>
#include "ChunkData.h"
#include "DataStructures.h"

namespace terrain {
	typedef std::unordered_map<
		utils::Vector3<int64>,
		TSharedRef<ChunkData>
	> ChunkCache;

	/**
	 * This class will load data related to a particular chunk, or serve it from
	 * memory if it has been cached. It will also invoke the terrain generator if
	 * the chunk hasn't be generated yet.
	 */
	class ChunkLoader {
	private:
		ChunkCache LoadedChunkCache;

		uint64 Seed;
		utils::Vector3<int64> ChunkSize;

		TSharedPtr<ChunkData> LoadChunkFromDisk(
			const utils::Vector3<int64> & offset);
		TSharedRef<ChunkData> GenerateMissingChunk(
			const utils::Vector3<int64> & offset);

	public:
		ChunkLoader();
		~ChunkLoader();

		void InitializeChunkLoader(
			const utils::Vector3<uint64> & chunkSize, uint64 seed);
		TSharedRef<ChunkData> GetChunkAt(const utils::Vector3<int64> & offset);
	};
}

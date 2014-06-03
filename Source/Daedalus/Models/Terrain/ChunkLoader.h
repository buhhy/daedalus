#pragma once

#include <unordered_map>
#include "ChunkData.h"
#include "TerrainDataStructures.h"
#include "DataStructures.h"

namespace terrain {
	typedef std::unordered_map<
		utils::Vector3<int64>,
		TSharedRef<ChunkData>
	> ChunkCache;

	/**
	 * This class will load data related to a particular chunk, or serve it from
	 * memory if it has been cached. It will also invoke the terrain generator if
	 * the chunk hasn't be generated yet. This class will most likely run on
	 * the server-side.
	 */
	class ChunkLoader {
	private:
		ChunkCache LoadedChunkCache;

		TerrainGeneratorParameters TerrainGenParams;

		TSharedPtr<ChunkData> LoadChunkFromDisk(
			const utils::Vector3<int64> & offset);
		TSharedRef<ChunkData> GenerateMissingChunk(
			const utils::Vector3<int64> & offset);

		//void RunDiamondSquare(ChunkData & data);
		void SetDefaultHeight(ChunkData & data, int32 height);

	public:
		ChunkLoader(const TerrainGeneratorParameters & params);
		~ChunkLoader();

		const TerrainGeneratorParameters & GetGeneratorParameters() const;
		TSharedRef<ChunkData> GetChunkAt(const utils::Vector3<int64> & offset);
	};
}

#pragma once

#include "ChunkData.h"
#include "TerrainDataStructures.h"
#include <Utilities/DataStructures.h>

#include <unordered_map>

namespace terrain {
	typedef std::unordered_map<
		utils::Vector3<int64_t>,
		std::shared_ptr<ChunkData>
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

		std::shared_ptr<ChunkData> LoadChunkFromDisk(const ChunkOffsetVector & offset);
		std::shared_ptr<ChunkData> GenerateMissingChunk(const ChunkOffsetVector & offset);

		//void RunDiamondSquare(ChunkData & data);
		void SetDefaultHeight(ChunkData & data, int32_t height);

	public:
		ChunkLoader(const TerrainGeneratorParameters & params);
		~ChunkLoader();

		const TerrainGeneratorParameters & GetGeneratorParameters() const;
		std::shared_ptr<ChunkData> GetChunkAt(const ChunkOffsetVector & offset);
	};
}

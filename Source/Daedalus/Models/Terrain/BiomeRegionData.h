#pragma once

#include "TerrainDataStructures.h"
#include "DataStructures.h"
#include "Delaunay.h"

#include <array>
#include <vector>
#include <unordered_map>

namespace terrain {
	typedef utils::Vector2<> BiomeCellVertex;
	typedef std::unordered_map<uint64, BiomeCellVertex> PointMap;
	typedef std::vector<uint64> PointIds;

	/**
	 * Each biome cell contains a list of 2D points that will be used in a Delaunay
	 * triangulation. Because we need to merge Delaunay triangulations in each region,
	 * we need a buffer border around each region that isn't finalized until that buffer
	 * border has been used in an adjacent region merge.
	 */
	struct BiomeCellData {
		PointIds PointIds;
		bool IsFinalized;

		BiomeCellData(): IsFinalized(false) {}

		~BiomeCellData() {}

		inline void AddPoint(const uint64 id) { PointIds.push_back(id); }
	};

	typedef utils::Tensor2<BiomeCellData> BiomePointField;

	/**
	 * Biomes are generated on a 2D plane separate from the terrain generation. Biome shapes
	 * are created using a tiled Delaunay triangulation. Each biome region is generated
	 * at a larger scale than terrain chunks and will be subdivided evenly into many biome
	 * cells. Each biome cell contains a least 1 Delaunay point for more even triangulations.
	 */
	class BiomeRegionData {
	private:
		uint64 CurrentVertexId;
		inline uint64 GetNextId() { return CurrentVertexId++; }

	public:
		PointMap Points;
		BiomePointField PointDistribution;
		utils::DelaunayGraph DelaunayGraph;

		// Indicates whether the 8 neighboring regions have been generated yet, (0, 0) is
		// bottom left, (2, 2) is top right.
		utils::Tensor2<bool> NeighborsLoaded;

		uint32 BiomeGridSize;                     // Size of the biome in grid cells
		BiomeRegionOffsetVector BiomeOffset;      // Biome offset from (0,0)

		BiomeRegionData(
			const uint16 buffer,
			const uint32 biomeSize,
			const BiomeRegionOffsetVector & biomeOffset);

		~BiomeRegionData() {}

		uint64 InsertPoint(const uint64 x, const uint64 y, const utils::Vector2<> & point);
		std::tuple<uint64, BiomeRegionGridVector, double> FindNearestPoint(
			utils::Vector2<> offset) const;
	};
}

#pragma once

#include "TerrainDataStructures.h"
#include "DataStructures.h"
#include "Delaunay.h"

#include <unordered_map>

namespace terrain {
	typedef std::unordered_map<uint16, utils::delaunay::Vertex *> PointSet;

	/**
	 * Each biome cell contains a list of 2D points that will be used in a Delaunay
	 * triangulation. Because we need to merge Delaunay triangulations in each region,
	 * we need a buffer border around each region that isn't finalized until that buffer
	 * border has been used in an adjacent region merge.
	 */
	struct BiomeCellData {
	private:
		uint16 VertexIdCount;
	public:
		PointSet Points;
		bool IsFinalized;

		BiomeCellData(): VertexIdCount(0), IsFinalized(false) {}

		~BiomeCellData() {
			for (auto it : Points)
				delete it.second;
		}

		utils::delaunay::Vertex * AddPoint(const utils::Vector2<> & point) {
			auto id = VertexIdCount ++;
			auto newVertex = new utils::delaunay::Vertex(point, id);
			Points.insert({ id, newVertex });
			return newVertex;
		}
	};

	typedef utils::Tensor2<BiomeCellData> BiomePointField;

	/**
	 * Biomes are generated on a 2D plane separate from the terrain generation. Biome shapes
	 * are created using a tiled Delaunay triangulation. Each biome region is generated
	 * at a larger scale than terrain chunks and will be subdivided evenly into many biome
	 * cells. Each biome cell contains a least 1 Delaunay point for more even triangulations.
	 */
	struct BiomeRegionData {
		BiomePointField PointDistribution;
		utils::delaunay::DelaunayGraph DelaunayGraph;

		BiomeSizeVector BiomeGridSize;      // Size of the biome in grid cells
		BiomeOffsetVector BiomeOffset;      // Biome offset from (0,0)

		BiomeRegionData(
			const uint16 buffer,
			const BiomeSizeVector & biomeSize,
			const BiomeOffsetVector & biomeOffset
		) : BiomeGridSize(biomeSize),
			BiomeOffset(biomeOffset),
			PointDistribution(biomeSize)
		{
			// Create buffer border around biome region
			for (auto y = PointDistribution.Depth - buffer - 1; y >= buffer; y--) {
				for (auto x = PointDistribution.Width - buffer - 1; x >= buffer; x--)
					PointDistribution.Get(x, y).IsFinalized = true;
			}
		}

		~BiomeRegionData() {}

		utils::delaunay::Vertex * InsertPoint(
			const uint64 x,
			const uint64 y,
			const utils::Vector2<> & point
		) {
			return PointDistribution.Get(x, y).AddPoint(point);
		}
	};
}

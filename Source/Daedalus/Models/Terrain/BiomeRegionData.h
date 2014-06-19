#pragma once

#include "TerrainDataStructures.h"
#include "DataStructures.h"
#include "Delaunay.h"

#include <array>
#include <vector>

namespace terrain {
	typedef std::vector<utils::Vector2<> > PointSet;

	/**
	 * Each biome cell contains a list of 2D points that will be used in a Delaunay
	 * triangulation. Because we need to merge Delaunay triangulations in each region,
	 * we need a buffer border around each region that isn't finalized until that buffer
	 * border has been used in an adjacent region merge.
	 */
	struct BiomeCellData {
	public:
		PointSet Points;
		bool IsFinalized;

		BiomeCellData(): IsFinalized(false) {}

		~BiomeCellData() {}

		void AddPoint(const utils::Vector2<> & point) {
			Points.push_back(point);
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
		utils::DelaunayGraph DelaunayGraph;

		// Indicates whether the 8 neighboring regions have been generated yet, the array is
		// ordered as follows: [ top, top-right, right, bottom-right, bottom, bottom-left,
		// left, top-left ], or [ Y+, XY+, X+, X+Y-, Y-, XY-, X-, X-Y+ ]
		std::array<bool, 8> NeighborsLoaded;

		BiomeSizeVector BiomeGridSize;      // Size of the biome in grid cells
		BiomeOffsetVector BiomeOffset;      // Biome offset from (0,0)

		BiomeRegionData(
			const uint16 buffer,
			const BiomeSizeVector & biomeSize,
			const BiomeOffsetVector & biomeOffset
		) : BiomeGridSize(biomeSize),
			BiomeOffset(biomeOffset),
			PointDistribution(biomeSize),
			NeighborsLoaded(std::array<bool, 8>())
		{
			auto toY = PointDistribution.Depth - buffer - 1;
			auto toX = PointDistribution.Width - buffer - 1;
			// Create buffer border around biome region
			for (auto y = buffer; y <= toY; y++) {
				for (auto x = buffer; x <= toX; x++)
					PointDistribution.Get(x, y).IsFinalized = true;
			}
		}

		~BiomeRegionData() {}

		void InsertPoint(
			const uint64 x,
			const uint64 y,
			const utils::Vector2<> & point
		) {
			PointDistribution.Get(x, y).AddPoint(point);
		}
	};
}

// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TerrainDataStructures.h"
#include "DataStructures.h"
#include "Delaunay.h"

#include <vector>

namespace terrain {
	/**
	 * Biomes are generated on a 2D plane separate from the terrain generation. Biome shapes
	 * are created using a tiled Delaunay triangulation. Each biome chunk is generated
	 * at a larger scale than terrain chunks and will contain many biomes. The chunk is
	 * subdivided into smaller grid cells, each cell containing a least 1 Delaunay point,
	 * with a specified maximum number of points per cell.
	 */
	struct BiomeRegionData {
		std::vector<utils::Vector2<double> > PointSet;
		utils::delaunay::DelaunayGraph DelaunayGraph;

		BiomeSizeVector BiomeGridSize;			// Size of the biome in grid cells
		BiomeOffsetVector BiomeOffset;			// Biome offset from (0,0)

		BiomeRegionData(
			const BiomeSizeVector & biomeSize,
			const BiomeOffsetVector & biomeOffset
		) : BiomeGridSize(biomeSize),
			BiomeOffset(biomeOffset) {}

		~BiomeRegionData() {}
	};
}

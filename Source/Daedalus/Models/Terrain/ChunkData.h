// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TerrainDataStructures.h"
#include "DataStructures.h"

namespace terrain {
	/**
	 * This struct holds all the relevant data for a terrain chunk. Chunks are tileable in
	 * all 3 dimensions, allowing for infinite worlds in height, depth and width. The axes
	 * are set up as follows: X -> width, Y -> depth, Z -> height.
	 */
	struct ChunkData {
		utils::Tensor3<float> DensityData;
		utils::Tensor3<uint64> MaterialData;

		ChunkSizeVector ChunkGridSize;			// Size of the chunk in polygons
		ChunkSizeVector ChunkFieldSize;			// Size of the chunk scalar field
		ChunkOffsetVector ChunkOffset;

		ChunkData(
			const ChunkSizeVector & chunkSize,
			const ChunkOffsetVector & chunkOffset
		) : ChunkGridSize(chunkSize),
			ChunkFieldSize(chunkSize + uint16(1)),
			DensityData(chunkSize + uint16(1), 0),
			MaterialData(chunkSize + uint16(1), 0),
			ChunkOffset(chunkOffset) {}

		~ChunkData() {}
	};
}

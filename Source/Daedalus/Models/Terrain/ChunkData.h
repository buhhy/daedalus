// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TerrainDataStructures.h"
#include <Utilities/DataStructures.h>

namespace terrain {
	/**
	 * This struct holds all the relevant data for a terrain chunk. Chunks are tileable in
	 * all 3 dimensions, allowing for infinite worlds in height, depth and width. The axes
	 * are set up as follows: X -> width, Y -> depth, Z -> height.
	 */
	struct ChunkData {
		utils::Tensor3<float> DensityData;
		utils::Tensor3<Uint64> MaterialData;

		Uint32 ChunkGridSize;			// Size of the chunk in polygons
		Uint32 ChunkFieldSize;			// Size of the chunk scalar field
		ChunkOffsetVector ChunkOffset;

		ChunkData(
			const Uint32 chunkSize,
			const ChunkOffsetVector & chunkOffset
		) : ChunkGridSize(chunkSize),
			ChunkFieldSize(chunkSize + 1),
			DensityData(chunkSize + 1, 0),
			MaterialData(chunkSize + 1, 0),
			ChunkOffset(chunkOffset) {}
		
		~ChunkData() {}
	};
}

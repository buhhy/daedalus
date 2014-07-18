// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <memory>

namespace terrain {
	/**
	 * This struct holds all the relevant data for a terrain chunk. Chunks are tileable in
	 * all 3 dimensions, allowing for infinite worlds in height, depth and width. The axes
	 * are set up as follows: X -> width, Y -> depth, Z -> height.
	 */
	struct ChunkData {
		/*
		 The chunk grid measurement counts cubic volumes in the cubic chunk. The density data
		 and material data on the other hand, are denoted at each vertex in the cubic chunk. To
		 convert from vertices to cubic volumes, we use the marching cubes algorithm. However
		 this means we need an extra row, column, and depth of vertex datum. A 16x16x16 chunk
		 requires a 17x17x17 vertex field. We need to tile chunks though so there are 2 options,
		 have a 17x17x17 field where the 17th points are synchronized, or have a 16x16x16
		 vertex field where the 17th points are retrieved upon use. I am going with the latter
		 approach because it seems conceptually simpler, at the potential cost of performance.

		 The density and material data field is the dual of the ingame grid.
		 */
		utils::Tensor3D<float> DensityData;
		utils::Tensor3D<Uint64> MaterialData;

		Uint32 ChunkGridSize;           // Size of the chunk in polygons
		Uint32 ChunkFieldSize;          // Size of the chunk scalar field
		ChunkOffsetVector ChunkOffset;

		ChunkData(
			const Uint32 chunkSize,
			const ChunkOffsetVector & chunkOffset
		) : ChunkGridSize(chunkSize),
			ChunkFieldSize(chunkSize),
			DensityData(chunkSize, 0),
			MaterialData(chunkSize, 0),
			ChunkOffset(chunkOffset) {}
		
		~ChunkData() {}
	};

	using ChunkDataPtr = std::shared_ptr<ChunkData>;
}

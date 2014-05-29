// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DataStructures.h"

/**
* This class holds all the relevant data for a terrain chunk. Chunks are tileable in
* all 3 dimensions, allowing for infinite worlds in height, depth and width. The axes
* are set up as follows: X -> width, Y -> depth, Z -> height.
*/
class Chunk {
private:
	utils::Tensor3<float> DensityData;
	utils::Tensor3<uint64> MaterialData;

	utils::Vector3<uint64> ChunkSize;
	utils::Vector3<int64> ChunkOffset;

	uint64 Seed;

	void RunDiamondSquare();
	void SetDefaultHeight(uint32 height);

public:
	Chunk();
	~Chunk();

	void InitializeChunk(
		const utils::Vector3<uint64> & chunkSize,
		const utils::Vector3<int64> & chunkOffset,
		uint64 seed);

	const utils::Vector3<uint64> & Size() const;
	const utils::Tensor3<float> & Density() const;
};

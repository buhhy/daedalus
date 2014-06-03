#pragma once

#include "DataStructures.h"

namespace terrain {
	typedef utils::Vector3<uint64> ChunkSizeVector;
	typedef utils::Vector3<int64> ChunkOffsetVector;

	struct TerrainGeneratorParameters {
		ChunkSizeVector ChunkPolygonSize;
		uint64 Seed;
		double ChunkScale;			// Maps chunk grid to real world coordinates

		const FVector ToRealCoordinates(const ChunkOffsetVector & offset) const {
			return FVector(
				(int64) ChunkPolygonSize.X * offset.X * ChunkScale,
				(int64) ChunkPolygonSize.Y * offset.Y * ChunkScale,
				(int64) ChunkPolygonSize.Z * offset.Z * ChunkScale);
		}

		const ChunkOffsetVector ToChunkCoordinates(const FVector & position) const {
			return ChunkOffsetVector(
				FMath::Floor(position.X / (ChunkScale * (int64) ChunkPolygonSize.X)),
				FMath::Floor(position.Y / (ChunkScale * (int64) ChunkPolygonSize.Y)),
				FMath::Floor(position.Z / (ChunkScale * (int64) ChunkPolygonSize.Z)));
		}
	};
}

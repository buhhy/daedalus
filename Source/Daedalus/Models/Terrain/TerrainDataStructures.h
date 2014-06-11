#pragma once

#include "DataStructures.h"

namespace terrain {
	typedef utils::Vector3<uint64> ChunkSizeVector;
	typedef utils::Vector3<int64> ChunkOffsetVector;

	struct TerrainGeneratorParameters {
		ChunkSizeVector ChunkGridCellSize;
		int64 Seed;
		double ChunkScale;			// Maps chunk grid to real world coordinates

		const FVector ToRealCoordinates(const ChunkOffsetVector & offset) const {
			return FVector(
				(int64) ChunkGridCellSize.X * offset.X * ChunkScale,
				(int64) ChunkGridCellSize.Y * offset.Y * ChunkScale,
				(int64) ChunkGridCellSize.Z * offset.Z * ChunkScale);
		}

		const ChunkOffsetVector ToChunkCoordinates(const FVector & position) const {
			return ChunkOffsetVector(
				FMath::Floor(position.X / (ChunkScale * (int64) ChunkGridCellSize.X)),
				FMath::Floor(position.Y / (ChunkScale * (int64) ChunkGridCellSize.Y)),
				FMath::Floor(position.Z / (ChunkScale * (int64) ChunkGridCellSize.Z)));
		}
	};


	typedef utils::Vector2<int64> BiomeOffsetVector;
	typedef utils::Vector2<uint64> BiomeSizeVector;

	struct BiomeGeneratorParameters {
		BiomeSizeVector BiomeGridCellSize;
		int64 Seed;
		uint64 BufferSize;			// Number of buffer cells for Delaunay graph merging
		uint16 MinPointsPerCell;
		uint16 MaxPointsPerCell;
		double BiomeScale;			// Maps biome grid to real world coordinates

		const FVector2D ToRealCoordinates(const BiomeOffsetVector & offset) const {
			return FVector2D(
				(int64) offset.X * BiomeScale,
				(int64) offset.Y * BiomeScale);
		}

		const BiomeOffsetVector ToChunkCoordinates(const FVector2D & position) const {
			return BiomeOffsetVector(
				FMath::Floor(position.X / BiomeScale),
				FMath::Floor(position.Y / BiomeScale));
		}
	};
}

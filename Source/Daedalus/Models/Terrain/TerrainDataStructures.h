#pragma once

#include "DataStructures.h"

namespace terrain {
	typedef utils::Vector3<uint16> ChunkSizeVector;
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


	typedef utils::Vector2<int64> BiomeRegionOffsetVector;   // Offset vector for biome regions
	typedef utils::Vector2<uint16> BiomeRegionSizeVector;    // TODO: remove this
	typedef utils::Vector2<uint16> BiomeRegionGridVector;    // Offset vector within a region
	typedef std::pair<BiomeRegionOffsetVector, uint64> BiomeVertexId;

	struct BiomeGeneratorParameters {
		BiomeRegionSizeVector BiomeGridCellSize;
		int64 Seed;
		uint64 BufferSize;			// Number of buffer cells for Delaunay graph merging
		uint16 MinPointsPerCell;
		uint16 MaxPointsPerCell;
		double BiomeScale;			// Maps biome grid to real world coordinates

		inline const utils::Vector2<> ToRealCoordinates(
			const BiomeRegionOffsetVector & offset
		) const {
			return utils::Vector2<>(
				(int64) offset.X * BiomeScale,
				(int64) offset.Y * BiomeScale);
		}

		inline const BiomeRegionOffsetVector ToBiomeRegionCoordinates(
			const utils::Vector2<> & position
		) const {
			return BiomeRegionOffsetVector(
				FMath::Floor(position.X / BiomeScale),
				FMath::Floor(position.Y / BiomeScale));
		}

		inline const utils::Vector2<> GetInnerRegionPosition(
			const utils::Vector2<> & position,
			const BiomeRegionOffsetVector & regionOffset
		) const {
			return utils::Vector2<>(
				position.X / BiomeScale - regionOffset.X,
				position.Y / BiomeScale - regionOffset.Y);
		}
	};
}

#pragma once

#include "DataStructures.h"

namespace terrain {
	typedef utils::Vector3<int64> ChunkOffsetVector;

	struct TerrainGeneratorParameters {
		uint32 GridCellCount;       // Number of grid cells along a single edge of the cube
		int64 Seed;
		double ChunkScale;			// Maps chunk grid to real world coordinates

		const FVector ToRealCoordinates(const ChunkOffsetVector & offset) const {
			return FVector(
				(int64) offset.X * ChunkScale,
				(int64) offset.Y * ChunkScale,
				(int64) offset.Z * ChunkScale);
		}

		const ChunkOffsetVector ToChunkCoordinates(const FVector & position) const {
			return ChunkOffsetVector(
				FMath::Floor(position.X / ChunkScale),
				FMath::Floor(position.Y / ChunkScale),
				FMath::Floor(position.Z / ChunkScale));
		}
	};


	typedef utils::Vector2<int64> BiomeRegionOffsetVector;   // Offset vector for biome regions
	typedef utils::Vector2<uint16> BiomeRegionGridVector;    // Offset vector within a region
	typedef std::pair<BiomeRegionOffsetVector, uint64> BiomeVertexId;

	struct BiomeGeneratorParameters {
		uint32 GridCellCount;       // Number of grid cells along a single edge of the square
		int64 Seed;
		uint64 BufferSize;          // Number of buffer cells for Delaunay graph merging
		uint16 MinPointsPerCell;
		uint16 MaxPointsPerCell;
		double BiomeScale;          // Maps biome grid to real world coordinates

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

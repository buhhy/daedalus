#pragma once

#include <Utilities/DataStructures.h>

namespace terrain {
	typedef utils::Vector3D<Int64> ChunkOffsetVector;

	struct TerrainGeneratorParameters {
		Uint32 GridCellCount;       // Number of grid cells along a single edge of the cube
		Int64 Seed;
		double ChunkScale;			// Maps chunk grid to real world coordinates

		const utils::Vector3D<> ToRealCoordinates(const ChunkOffsetVector & offset) const {
			return utils::Vector3D<>(
				offset.X * ChunkScale,
				offset.Y * ChunkScale,
				offset.Z * ChunkScale);
		}

		const ChunkOffsetVector ToChunkCoordinates(const utils::Vector3D<> & position) const {
			return ChunkOffsetVector(
				(Int64) std::floor(position.X / ChunkScale),
				(Int64) std::floor(position.Y / ChunkScale),
				(Int64) std::floor(position.Z / ChunkScale));
		}
	};


	typedef utils::Vector2D<Int64> BiomeRegionOffsetVector;   // Offset vector for biome regions
	typedef utils::Vector2D<Uint16> BiomeRegionGridVector;    // Offset vector within a region
	typedef std::pair<BiomeRegionOffsetVector, Uint64> BiomeId;

	struct BiomeGeneratorParameters {
		Uint32 GridCellCount;       // Number of grid cells along a single edge of the square
		Int64 Seed;
		Uint16 BufferSize;          // Number of buffer cells for Delaunay graph merging
		Uint16 MinPointsPerCell;
		Uint16 MaxPointsPerCell;
		double BiomeScale;          // Maps biome grid to real world coordinates

		inline const utils::Vector2D<> ToRealCoordinates(
			const BiomeRegionOffsetVector & offset
		) const {
			return utils::Vector2D<>(offset.X * BiomeScale, offset.Y * BiomeScale);
		}

		inline const BiomeRegionOffsetVector ToBiomeRegionCoordinates(
			const utils::Vector2D<> & position
		) const {
			return BiomeRegionOffsetVector(
				(Int64) std::floor(position.X / BiomeScale),
				(Int64) std::floor(position.Y / BiomeScale));
		}

		inline const utils::Vector2D<> GetInnerRegionPosition(
			const utils::Vector2D<> & position,
			const BiomeRegionOffsetVector & regionOffset
		) const {
			return utils::Vector2D<>(
				position.X / BiomeScale - regionOffset.X,
				position.Y / BiomeScale - regionOffset.Y);
		}
	};
}

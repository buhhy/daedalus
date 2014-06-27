#pragma once

#include <Utilities/DataStructures.h>

namespace terrain {
	typedef utils::Vector3<int64_t> ChunkOffsetVector;

	struct TerrainGeneratorParameters {
		uint32_t GridCellCount;       // Number of grid cells along a single edge of the cube
		int64_t Seed;
		double ChunkScale;			// Maps chunk grid to real world coordinates

		const utils::Vector3<> ToRealCoordinates(const ChunkOffsetVector & offset) const {
			return utils::Vector3<>(
				offset.X * ChunkScale,
				offset.Y * ChunkScale,
				offset.Z * ChunkScale);
		}

		const ChunkOffsetVector ToChunkCoordinates(const utils::Vector3<> & position) const {
			return ChunkOffsetVector(
				(int64_t) std::floor(position.X / ChunkScale),
				(int64_t) std::floor(position.Y / ChunkScale),
				(int64_t) std::floor(position.Z / ChunkScale));
		}
	};


	typedef utils::Vector2<int64_t> BiomeRegionOffsetVector;   // Offset vector for biome regions
	typedef utils::Vector2<uint16_t> BiomeRegionGridVector;    // Offset vector within a region
	typedef std::pair<BiomeRegionOffsetVector, uint64_t> BiomeId;

	struct BiomeGeneratorParameters {
		uint32_t GridCellCount;       // Number of grid cells along a single edge of the square
		int64_t Seed;
		uint16_t BufferSize;          // Number of buffer cells for Delaunay graph merging
		uint16_t MinPointsPerCell;
		uint16_t MaxPointsPerCell;
		double BiomeScale;          // Maps biome grid to real world coordinates

		inline const utils::Vector2<> ToRealCoordinates(
			const BiomeRegionOffsetVector & offset
		) const {
			return utils::Vector2<>(offset.X * BiomeScale, offset.Y * BiomeScale);
		}

		inline const BiomeRegionOffsetVector ToBiomeRegionCoordinates(
			const utils::Vector2<> & position
		) const {
			return BiomeRegionOffsetVector(
				(int64_t) std::floor(position.X / BiomeScale),
				(int64_t) std::floor(position.Y / BiomeScale));
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

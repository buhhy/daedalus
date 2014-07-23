#pragma once

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

namespace terrain {
	using ChunkOffsetVector = utils::Vector3D<Int64>;
	using ChunkPositionVector = std::pair<ChunkOffsetVector, utils::Point3D>;

	struct TerrainGeneratorParameters {
		Uint32 GridCellCount;       // Number of grid cells along a single edge of the cube
		Int64 Seed;
		double ChunkScale;          // Maps chunk grid to real world coordinates

		const utils::Point3D ToRealCoordinates(const ChunkOffsetVector & offset) const {
			return { offset.X * ChunkScale, offset.Y * ChunkScale, offset.Z * ChunkScale };
		}

		const utils::Point3D ToRealCoordinates(const ChunkPositionVector & position) const {
			return ToRealCoordinates(position.first) + position.second * ChunkScale;
		}

		const utils::Point3D GetChunkInnerPosition(const ChunkPositionVector & position) const {
			return position.second * ChunkScale;
		}

		const ChunkOffsetVector ToChunkCoordinates(const utils::Point3D & position) const {
			return ChunkOffsetVector(
				(Int64) std::floor(position.X / ChunkScale),
				(Int64) std::floor(position.Y / ChunkScale),
				(Int64) std::floor(position.Z / ChunkScale));
		}
	};


	using BiomeRegionOffsetVector = utils::Vector2D<Int64>;   // Offset vector for biome regions
	using BiomeRegionGridVector = utils::Vector2D<Uint16>;    // Offset vector within a region
	// UID referencing an entity within a biome region
	using BiomeId = std::pair<BiomeRegionOffsetVector, Uint64>;
	// Positioning component for referencing a point within a biome region
	using BiomePositionVector = std::pair<BiomeRegionOffsetVector, utils::Vector2D<>>;

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

		inline const BiomePositionVector ToBiomeRegionCoordinates(
			const utils::Vector2D<> & point
		) const {
			const auto offset = BiomeRegionOffsetVector(
				(Int64) std::floor(point.X / BiomeScale),
				(Int64) std::floor(point.Y / BiomeScale));
			const auto position = utils::Vector2D<>(
				point.X / BiomeScale - offset.X,
				point.Y / BiomeScale - offset.Y);
			return BiomePositionVector(offset, position);
		}
	};
}

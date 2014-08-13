#pragma once

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

namespace terrain {
	using ChunkOffsetVector = utils::Vector3D<Int64>;       // Offset vector for entire chunk
	using ChunkGridIndexVector = utils::Vector3D<Uint16>;   // Index vector within the chunk grid
	using ChunkPositionVector = std::pair<ChunkOffsetVector, utils::Point3D>;

	struct TerrainGeneratorParameters {
		const Uint16 GridCellCount;       // Number of grid cells along a single edge of the cube
		const Int64 Seed;
		const double ChunkScale;          // Size of each chunk in centimetres
		const double ChunkGridUnitSize;   // Size of each grid unit in centimetres

		TerrainGeneratorParameters(
			const Uint16 cellCount,
			const Int64 seed,
			const double chunkSize
		) : GridCellCount(cellCount), Seed(seed),
			ChunkScale(chunkSize), ChunkGridUnitSize(chunkSize / cellCount)
		{}

		/*
		 Chunk coordinates uses a integer vector portion to indicate which chunk a point belongs,
		 and a double vector portion with a range of [0, 1] to indicate the position within the
		 chunk itself. This helps to preserve accuracy at higher chunk positions.
		 */

		utils::Point3D ToRealCoordSpace(const ChunkOffsetVector & offset) const {
			return { offset.X * ChunkScale, offset.Y * ChunkScale, offset.Z * ChunkScale };
		}

		/**
		 * Converts the entire chunk position vector into real world coordinates.
		 */
		utils::Point3D ToRealCoordSpace(const ChunkPositionVector & position) const {
			return ToRealCoordSpace(position.first) + ToRealCoordSpace(position.second);
		}
		
		/**
		 * Retrieves the inner position of the chunk position vector in real world coordinates.
		 */
		utils::Point3D ToRealCoordSpace(const utils::Point3D & virtualPos) const {
			return virtualPos * ChunkGridUnitSize;
		}

		/**
		 * Converts real world coordinates to chunk coordinates.
		 */
		ChunkPositionVector ToGridCoordSpace(const utils::Point3D & point) const {
			const auto offset = ChunkOffsetVector(
				(Int64) utils::EFloor(point.X / ChunkScale),
				(Int64) utils::EFloor(point.Y / ChunkScale),
				(Int64) utils::EFloor(point.Z / ChunkScale));
			const auto position = utils::Point3D(
				GridCellCount * (point.X / ChunkScale - offset.X),
				GridCellCount * (point.Y / ChunkScale - offset.Y),
				GridCellCount * (point.Z / ChunkScale - offset.Z));
			return ChunkPositionVector(offset, position);
		}

		/**
		 * Converts real world coordinates to chunk coordinates.
		 */
		ChunkPositionVector ToGridCoordSpace(
			const utils::Point3D & position,
			const ChunkOffsetVector & anchor
		) const {
			auto vec = ToGridCoordSpace(position);
			return ChunkPositionVector(anchor, {
				GridCellCount * (vec.first.X - anchor.X) + vec.second.X,
				GridCellCount * (vec.first.Y - anchor.Y) + vec.second.Y,
				GridCellCount * (vec.first.Z - anchor.Z) + vec.second.Z
			});
		}

		/**
		 * Ensures the provided position vector has an inner position of [0, GridCellCount).
		 */
		ChunkPositionVector Normalize(const ChunkPositionVector & vect) const {
			if (vect.second.IsBoundedBy(-DBL_EPSILON, GridCellCount - DBL_EPSILON))
				return vect;
			const double x = vect.second.X;
			const double y = vect.second.Y;
			const double z = vect.second.Z;
			const utils::Vector3D<Int64> offset(
				utils::EFloor(x / GridCellCount),
				utils::EFloor(y / GridCellCount),
				utils::EFloor(z / GridCellCount));
			const utils::Point3D itemPosition(
				x - offset.X * GridCellCount,
				y - offset.Y * GridCellCount,
				z - offset.Z * GridCellCount);
			return ChunkPositionVector(vect.first + offset, itemPosition);
		}

		bool WithinGridBounds(const utils::Point3D & point) const {
			return point.IsBoundedBy(0, GridCellCount);
		}
	};


	using BiomeRegionOffsetVector = utils::Vector2D<Int64>;      // Offset vector for biome regions
	using BiomeRegionGridIndexVector = utils::Vector2D<Uint16>;  // Index vector within a region
	// UID referencing an entity within a biome region
	using BiomeId = std::pair<BiomeRegionOffsetVector, Uint64>;
	// Positioning component for referencing a point within a biome region
	using BiomePositionVector = std::pair<BiomeRegionOffsetVector, utils::Vector2D<>>;

	struct BiomeGeneratorParameters {
		Uint16 GridCellCount;       // Number of grid cells along a single edge of the square
		Int64 Seed;
		Uint16 BufferSize;          // Number of buffer cells for Delaunay graph merging
		Uint16 MinPointsPerCell;
		Uint16 MaxPointsPerCell;
		double BiomeScale;          // Maps biome grid to real world coordinates

		inline utils::Vector2D<> ToRealCoordSpace(
			const BiomeRegionOffsetVector & offset
		) const {
			return utils::Vector2D<>(offset.X * BiomeScale, offset.Y * BiomeScale);
		}

		inline BiomePositionVector ToBiomeRegionCoordinates(
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

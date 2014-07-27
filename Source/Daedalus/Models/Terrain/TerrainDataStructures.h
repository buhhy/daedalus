#pragma once

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

namespace terrain {
	using ChunkOffsetVector = utils::Vector3D<Int64>;       // Offset vector for entire chunk
	using ChunkGridIndexVector = utils::Vector3D<Uint16>;   // Index vector within the chunk grid
	using ChunkPositionVector = std::pair<ChunkOffsetVector, utils::Point3D>;

	struct TerrainGeneratorParameters {
		Uint16 GridCellCount;       // Number of grid cells along a single edge of the cube
		Int64 Seed;
		double ChunkScale;          // Maps chunk grid to real world coordinates

		/*
		 Chunk coordinates uses a integer vector portion to indicate which chunk a point belongs,
		 and a double vector portion with a range of [0, 1] to indicate the position within the
		 chunk itself. This helps to preserve accuracy at higher chunk positions.
		 */

		const utils::Point3D ToRealCoordinates(const ChunkOffsetVector & offset) const {
			return { offset.X * ChunkScale, offset.Y * ChunkScale, offset.Z * ChunkScale };
		}

		/**
		 * Converts the entire chunk position vector into real world coordinates.
		 */
		const utils::Point3D ToRealCoordinates(const ChunkPositionVector & position) const {
			return ToRealCoordinates(position.first) + GetChunkInnerPosition(position);
		}

		/**
		 * Retrieves the inner position of the chunk position vector in real world coordinates.
		 */
		const utils::Point3D GetChunkInnerPosition(const ChunkPositionVector & position) const {
			return position.second * ChunkScale;
		}

		/**
		 */
		const utils::Point3D GetChunkInnerPosition(const ChunkGridIndexVector & point) const {
			return {
				ChunkScale * (double) point.X / GridCellCount,
				ChunkScale * (double) point.Y / GridCellCount,
				ChunkScale * (double) point.Z / GridCellCount
			};
		}

		/**
		 * @param position Chunk inner position vector between [0, 1].
		 * @return The inner grid indices of the given position point.
		 */
		const ChunkGridIndexVector GetChunkGridIndicies(const utils::Point3D & position) const {
			return {
				Uint16(position.X * GridCellCount),
				Uint16(position.Y * GridCellCount),
				Uint16(position.Z * GridCellCount)
			};
		}

		/**
		 * Converts real world coordinates to chunk coordinates.
		 */
		const ChunkPositionVector ToChunkCoordinates(const utils::Point3D & point) const {
			const auto offset = ChunkOffsetVector(
				(Int64) std::floor(point.X / ChunkScale),
				(Int64) std::floor(point.Y / ChunkScale),
				(Int64) std::floor(point.Z / ChunkScale));
			const auto position = utils::Point3D(
				point.X / ChunkScale - offset.X,
				point.Y / ChunkScale - offset.Y,
				point.Z / ChunkScale - offset.Z);
			return ChunkPositionVector(offset, position);
		}

		/**
		 * Converts real world coordinates to chunk coordinates.
		 */
		const ChunkPositionVector ToChunkCoordinates(
			const utils::Point3D & position,
			const ChunkOffsetVector & anchor
		) const {
			auto vec = ToChunkCoordinates(position);
			return ChunkPositionVector(anchor, {
				vec.first.X - anchor.X + vec.second.X,
				vec.first.Y - anchor.Y + vec.second.Y,
				vec.first.Z - anchor.Z + vec.second.Z
			});
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

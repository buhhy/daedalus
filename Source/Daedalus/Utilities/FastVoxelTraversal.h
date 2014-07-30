#pragma once

#include <Utilities/Algebra/Algebra3D.h>

namespace utils {
	struct TraversalCollision {
		Vector3D<Int64> GridIndex;
		Vector3D<Int64> LastValidGridIndex;

		TraversalCollision() : GridIndex(0, 0, 0), LastValidGridIndex(0, 0, 0) {}
	};

	
	/**
	 * This algorithm effectively does an iterative raytrace through a discrete grid. The original
	 * algorithm can be found here: http://www.cse.yorku.ca/~amana/research/grid.pdf
	 */
	class FastVoxelTraversalIterator {
	private:
		const Vector3D<> GridCellSize;
		const Vector3D<Int64> LowerBound;
		const Vector3D<Int64> UpperBound;
		const bool bIsBounded;
		const Ray3D Ray;

		Vector3D<> StartingPoint;
		Vector3D<Int64> PreviousCell;
		Vector3D<Int64> CurrentCell;
		Vector3D<Int8> Step;
		Vector3D<double> TCurrent;
		Vector3D<double> TNext;
		Vector3D<double> TDelta;
		double DistanceLimit;

		bool bIsValid;

		void Initialize();

		inline void Gridize(
			utils::Vector3D<Int64> & out,
			const utils::Vector3D<double> & in
		) const {
			// Account for floating point error
			out.Reset(
				utils::EFloor(in.X / GridCellSize.X),
				utils::EFloor(in.Y / GridCellSize.Y),
				utils::EFloor(in.Z / GridCellSize.Z));
		}

	public:
		FastVoxelTraversalIterator(
			const Vector3D<> & gridCellSize,
			const Vector3D<Int64> & lowerBound,
			const Vector3D<Int64> & upperBound,
			const Ray3D & ray,
			const double distanceLimit);
		
		FastVoxelTraversalIterator(
			const Vector3D<> & gridCellSize,
			const Ray3D & ray,
			const double distanceLimit);

		void Next();
		inline bool IsValid() const { return bIsValid; }
		inline const Vector3D<Int64> & GetCurrentCell() const { return CurrentCell; }
		inline const Vector3D<Int64> & GetPreviousCell() const { return PreviousCell; }
	};
}

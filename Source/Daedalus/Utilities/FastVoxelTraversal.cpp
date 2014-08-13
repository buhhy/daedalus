#include <Daedalus.h>
#include "FastVoxelTraversal.h"

#include <algorithm>

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

namespace utils {
	FastVoxelTraversalIterator::FastVoxelTraversalIterator(
		const Vector3D<> & gridCellSize,
		const Vector3D<Int64> & lowerBound,
		const Vector3D<Int64> & upperBound,
		const Ray3D & ray,
		const double distanceLimit
	) : GridCellSize(gridCellSize),
		LowerBound(lowerBound),
		UpperBound(upperBound),
		Ray(ray),
		DistanceLimit(distanceLimit),
		bIsBounded(true),
		StartingPoint(0), PreviousCell(0), CurrentCell(0), Step(0),
		TCurrent(0), TNext(std::numeric_limits<double>::infinity()), TDelta(0),
		bIsValid(true)
	{
		const AxisAlignedBoundingBox3D boundingBox({
			GridCellSize.X * LowerBound.X,
			GridCellSize.Y * LowerBound.Y,
			GridCellSize.Z * LowerBound.Z
		}, {
			GridCellSize.X * UpperBound.X,
			GridCellSize.Y * UpperBound.Y,
			GridCellSize.Z * UpperBound.Z
		});

		// Make sure the ray intersects the bounding box.
		double tEntry;
		const bool doesEnter = boundingBox.RayIntersection(ray, &StartingPoint, &tEntry);
		DistanceLimit -= tEntry;

		// If the ray doesn't enter the chunk within the maximum allowed t-value, then
		// return false.
		bIsValid = doesEnter && DistanceLimit > DOUBLE_EPSILON;
		
		Gridize(CurrentCell, StartingPoint);
		if (tEntry > DOUBLE_EPSILON) {
			// The ray entered from outside the current grid, so we need to manually obtain the
			// cell prior to entering the bounding box.
			for (Uint8 i = 0; i < 3; i++) {
				// -1 if CurrentCell[i] == 0, 1 if CurrentCell[i] == 15
				const auto value = ((CurrentCell[i] + 1) * 2) / (UpperBound[i] - LowerBound[i]) - 1;
				PreviousCell[i] = CurrentCell[i] + value;
			}
		} else {
			PreviousCell = CurrentCell;
		}

		assert(!StartingPoint.IsBoundedBy(0, 1) && "AChunk::SolidIntersection: Invalid entry point");

		if (bIsValid)
			Initialize();
	}
		
	FastVoxelTraversalIterator::FastVoxelTraversalIterator(
		const Vector3D<> & gridCellSize,
		const Ray3D & ray,
		const double distanceLimit
	) : GridCellSize(gridCellSize),
		LowerBound(0, 0, 0),
		UpperBound(0, 0, 0),
		Ray(ray),
		DistanceLimit(distanceLimit),
		bIsBounded(false),
		StartingPoint(ray.Origin), PreviousCell(0), CurrentCell(0), Step(0),
		TCurrent(0), TNext(std::numeric_limits<double>::infinity()), TDelta(0),
		bIsValid(true)
	{
		Gridize(CurrentCell, StartingPoint);
		PreviousCell = CurrentCell;
		Initialize();
	}

	void FastVoxelTraversalIterator::Initialize() {
		Step.Reset(Sign(Ray.Direction.X), Sign(Ray.Direction.Y), Sign(Ray.Direction.Z));

		// Initialize the next t-values and the per-cell t deltas.
		for (Uint8 i = 0; i < 3; i++) {
			if (Step[i] < 0)
				TNext[i] = (CurrentCell[i] * GridCellSize[i] - StartingPoint[i]) / Ray.Direction[i];
			else if (Step[i] > 0)
				TNext[i] = ((CurrentCell[i] + 1) * GridCellSize[i] - StartingPoint[i]) / Ray.Direction[i];
			TDelta[i] = GridCellSize[i] / std::abs(Ray.Direction[i]);
		}
	}

	void FastVoxelTraversalIterator::Next() {
		if (bIsValid) {
			TCurrent = TNext;
			PreviousCell = CurrentCell;
			if (TNext.X < TNext.Y) {
				if (TNext.X < TNext.Z) {
					CurrentCell.X += Step.X;
					TNext.X += TDelta.X;
				} else {
					CurrentCell.Z += Step.Z;
					TNext.Z += TDelta.Z;
				}
			} else {
				if (TNext.Y < TNext.Z) {
					CurrentCell.Y += Step.Y;
					TNext.Y += TDelta.Y;
				} else {
					CurrentCell.Z += Step.Z;
					TNext.Z += TDelta.Z;
				}
			}
			if (bIsBounded && !CurrentCell.IsBoundedBy(LowerBound, UpperBound))
				bIsValid = false; // Not found.

			// Target is out of range
			if (TCurrent.X > DistanceLimit &&
					TCurrent.Y > DistanceLimit &&
					TCurrent.Z > DistanceLimit)
				bIsValid = false;
		}
	}
}

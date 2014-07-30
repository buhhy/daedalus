#pragma once

#include <Utilities/Algebra/Vector3D.h>

namespace utils {
	using Point3D = Vector3D<>;

	struct Triangle3D {
		Point3D Point1, Point2, Point3;

		Triangle3D(const Point3D & p1, const Point3D & p2, const Point3D & p3) :
			Point1(p1), Point2(p2), Point3(p3)
		{}
	};

	struct Ray3D {
		Point3D Origin;
		Vector3D<> Direction;

		Ray3D(const Point3D & origin, const Vector3D<> & direction) :
			Origin(origin), Direction(direction)
		{}
	};

	struct AxisAlignedBoundingBox3D {
		Point3D MinPoint;
		Point3D MaxPoint;

		AxisAlignedBoundingBox3D(const Point3D & min, const Point3D & max) :
			MinPoint(min), MaxPoint(max)
		{}

		/**
		 * This method WILL introduce floating point error into the intersection point. Make sure
		 * to do floating-point comparisons using epsilon.
		 */
		bool FindIntersection(
			const Ray3D & ray, Point3D * intersectPoint = NULL, double * tValue = NULL) const;
	};
}

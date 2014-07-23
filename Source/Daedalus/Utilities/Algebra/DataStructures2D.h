#pragma once

#include <Utilities/Algebra/Vector2D.h>
#include <Utilities/Algebra/Vector3D.h>
#include <Utilities/Algebra/Tensor2D.h>

namespace utils {
	using UVWVector = Vector3D<>;
	using Point3D = Vector3D<>;

	struct Circle2D {
		Vector2D<> Center;
		double Radius;

		Circle2D(const Vector2D<> center, const double radius) :
			Center(center), Radius(radius)
		{}
		Circle2D() : Circle2D({ 0, 0 }, 0) {}
	};

	struct Triangle2D {
		Vector2D<> Point1, Point2, Point3;

		Triangle2D(
			const Vector2D<> & p1,
			const Vector2D<> & p2,
			const Vector2D<> & p3
		) : Point1(p1), Point2(p2), Point3(p3) {}

		/**
		 * Retrieves the barycentric coordinates of the provided point.
		 * @Return true if the point is within this triangle, false if not
		 */
		bool GetBarycentricCoordinates(UVWVector & output, const Vector2D<> & point) const;
	};
}

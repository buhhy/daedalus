#include <Daedalus.h>
#include "DataStructures2D.h"

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

namespace utils {
	bool Triangle2D::GetBarycentricCoordinates(
		UVWVector & output, const Vector2D<> & point
	) const {
		// To use the UVW values, use this formula:
		// ivalue = u * v2 + v * v3 + w * v1
		// X is aliased to u, Y to v, Z to w
		Vector3D<> AC = Point3 - Point1;
		Vector3D<> CB = Point2 - Point3;
		Vector3D<> BA = Point1 - Point2;

		auto normal = (-BA).Cross<double>(AC);
		output.X = normal.Dot(Vector3D<>(point - Point1).Cross<double>(AC));
		if (output.X < 0) return false;

		output.Y = normal.Dot(Vector3D<>(point - Point2).Cross<double>(BA));
		if (output.Y < 0) return false;

		output.Z = normal.Dot(Vector3D<>(point - Point3).Cross<double>(CB));
		if (output.Z < 0) return false;

		double area2 = normal.Dot(normal);
		output /= area2;

		return true;
	}
}

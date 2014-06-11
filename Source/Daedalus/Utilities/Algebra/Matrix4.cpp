#include "Daedalus.h"
#include "DataStructures.h"
#include "Engine.h"

namespace utils {
	/**
	 * Return a matrix to represent a counterclockwise rotation of "angle"
	 * degrees around the axis "axis", where "axis" is one of the
	 * characters 'x', 'y', or 'z'.
	 */
	Matrix4<> CreateRotation(const double angle,  const Axis axis) {
		double radAngle = angle * utils::M_PI/180;
		switch (axis) {
			case AXIS_X:
				return Matrix4<>(
					1.0, 0.0, 0.0, 0.0,
					0.0, FMath::Cos(radAngle), -FMath::Sin(radAngle), 0.0,
					0.0, FMath::Sin(radAngle), FMath::Cos(radAngle), 0.0,
					0.0, 0.0, 0.0, 1.0);
			case AXIS_Y:
				return Matrix4<>(
					FMath::Cos(radAngle), 0.0, FMath::Sin(radAngle), 0.0,
					0.0, 1.0, 0.0, 0.0,
					-FMath::Sin(radAngle), 0.0, FMath::Cos(radAngle), 0.0,
					0.0, 0.0, 0.0, 1.0);
			case AXIS_Z:
				return Matrix4<>(
					FMath::Cos(radAngle), FMath::Sin(radAngle), 0.0, 0.0,
					-FMath::Sin(radAngle), FMath::Cos(radAngle), 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0);
			default:
				return Matrix4<>();
		}
	}

	/**
	 * Returns a rotation matrix built around the provided basis XYZ vectors.
	 */
	Matrix4<> CreateRotation(const Vector3<> & x,  const Vector3<> & y,  const Vector3<> & z) {
		return Matrix4<>(
			x.X, y.X, z.X, 0,
			x.Y, y.Y, z.Y, 0,
			x.Z, y.Z, z.Z, 0,
			0, 0, 0, 1);
	}

	// Return a matrix to represent a displacement of the given vector.
	Matrix4<> CreateTranslation(const Vector3<> & displacement) {
		return Matrix4<>(
			1., 0, 0, displacement.X,
			0, 1., 0, displacement.Y,
			0, 0, 1., displacement.Z,
			0, 0, 0, 1.);
	}

	// Return a matrix to represent a nonuniform scale with the given factors.
	Matrix4<> CreateScaling(const Vector3<> & scale) {
		return Matrix4<>(
			scale.X, 0, 0, 0,
			0, scale.Y, 0, 0,
			0, 0, scale.Z, 0,
			0, 0, 0, 1.);
	}
}

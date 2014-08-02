#include <Daedalus.h>
#include "Matrix4D.h"
#include <Utilities/Algebra/Algebra3D.h>

namespace utils {
	Matrix4D<> CreateRotation(const double angle, const Axis axis) {
		double radAngle = angle * MATH_PI/180;
		switch (axis) {
			case AXIS_X:
				return Matrix4D<>(
					1.0, 0.0, 0.0, 0.0,
					0.0, std::cos(radAngle), -std::sin(radAngle), 0.0,
					0.0, std::sin(radAngle), std::cos(radAngle), 0.0,
					0.0, 0.0, 0.0, 1.0);
			case AXIS_Y:
				return Matrix4D<>(
					std::cos(radAngle), 0.0, std::sin(radAngle), 0.0,
					0.0, 1.0, 0.0, 0.0,
					-std::sin(radAngle), 0.0, std::cos(radAngle), 0.0,
					0.0, 0.0, 0.0, 1.0);
			case AXIS_Z:
				return Matrix4D<>(
					std::cos(radAngle), std::sin(radAngle), 0.0, 0.0,
					-std::sin(radAngle), std::cos(radAngle), 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0);
			default:
				return Matrix4D<>();
		}
	}

	Matrix4D<> CreateRotation(const Vector3D<> & x,  const Vector3D<> & y,  const Vector3D<> & z) {
		return Matrix4D<>(
			x.X, y.X, z.X, 0,
			x.Y, y.Y, z.Y, 0,
			x.Z, y.Z, z.Z, 0,
			0, 0, 0, 1);
	}

	Matrix4D<> CreateTranslation(const Vector3D<> & displacement) {
		return Matrix4D<>(
			1., 0, 0, displacement.X,
			0, 1., 0, displacement.Y,
			0, 0, 1., displacement.Z,
			0, 0, 0, 1.);
	}

	Matrix4D<> CreateScaling(const Vector3D<> & scale) {
		return Matrix4D<>(
			scale.X, 0, 0, 0,
			0, scale.Y, 0, 0,
			0, 0, scale.Z, 0,
			0, 0, 0, 1.);
	}
	
		
	Basis3D GetBasisFrom(const Matrix4D<> & mat) {
		return Basis3D(
			Vector3D<>(mat.Get(0, 0), mat.Get(0, 1), mat.Get(0, 2)),
			Vector3D<>(mat.Get(1, 0), mat.Get(1, 1), mat.Get(1, 2)),
			Vector3D<>(mat.Get(2, 0), mat.Get(2, 1), mat.Get(2, 2)));
	}

	Vector3D<> GetTranslationVectorFrom(const Matrix4D<> & mat) {
		return { mat.Get(3, 0), mat.Get(3, 1), mat.Get(3, 2) };
	}
}

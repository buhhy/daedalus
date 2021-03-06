#pragma once

#include "Engine.h"
#include <Utilities/Algebra/DataStructures3D.h>

namespace utils {
	inline Vector2D<> ToVector2D(const FVector & fv) { return Vector2D<>(fv.X, fv.Y); }

	inline FVector ToFVector(const Vector2D<> & vec, const float z = 0.0) {
		return FVector(vec.X, vec.Y, z);
	}

	inline Vector3D<> ToVector3D(const FVector & fv) { return Vector3D<>(fv.X, fv.Y, fv.Z); }
	inline FVector ToFVector(const Vector3D<> & vec) { return FVector(vec.X, vec.Y, vec.Z); }
}

#pragma once

#include "Engine.h"
#include <Utilities/DataStructures.h>

namespace utils {
	inline Vector2<> ToVector2(const FVector & fv) { return Vector2<>(fv.X, fv.Y); }

	inline FVector ToFVector(const Vector2<> & vec, const float z = 0.0) {
		return FVector(vec.X, vec.Y, z);
	}

	inline Vector3<> ToVector3(const FVector & fv) { return Vector3<>(fv.X, fv.Y, fv.Z); }
	inline FVector ToFVector(const Vector3<> & vec) { return FVector(vec.X, vec.Y, vec.Z); }
}

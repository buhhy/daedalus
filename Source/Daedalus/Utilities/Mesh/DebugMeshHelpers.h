#pragma once

#include "DataStructures.h"

#include <vector>

namespace utils {
	std::vector<Triangle> CreatePoint(const Vector3<> & position, const float radius);
	std::vector<Triangle> CreateLine(
		const Vector3<> & startPoint, const Vector3<> & endPoint, const float radius);
}

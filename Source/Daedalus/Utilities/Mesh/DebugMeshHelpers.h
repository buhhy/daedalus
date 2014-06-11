#pragma once

#include "DataStructures.h"

#include <vector>

namespace utils {
	uint16 CreatePoint(
		std::vector<Triangle> & results, const Vector3<> & position, const float radius);
	uint16 CreateLine(
		std::vector<Triangle> & results, const Vector3<> & startPoint,
		const Vector3<> & endPoint, const float radius);
}

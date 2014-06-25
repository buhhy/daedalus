#pragma once

#include <Utilities/DataStructures.h>

#include <vector>

namespace utils {
	uint16_t CreatePoint(
		std::vector<Triangle> & results, const Vector3<> & position, const float radius);
	uint16_t CreateLine(
		std::vector<Triangle> & results, const Vector3<> & startPoint,
		const Vector3<> & endPoint, const float radius);
}

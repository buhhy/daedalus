#pragma once

#include <Utilities/Algebra/Algebra2.h>
#include <Utilities/Algebra/Algebra3.h>

#include <vector>

/**
 * These helper functions create helper meshes like lines and points for easier debugging.
 */
namespace utils {
	Uint16 CreatePoint(
		std::vector<Triangle> & results, const Vector3<> & position, const float radius);
	Uint16 CreateLine(
		std::vector<Triangle> & results, const Vector3<> & startPoint,
		const Vector3<> & endPoint, const float radius);
}

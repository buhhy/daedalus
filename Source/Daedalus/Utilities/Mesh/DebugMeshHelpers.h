#pragma once

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <vector>

/**
 * These helper functions create helper meshes like lines and points for easier debugging.
 */
namespace utils {
	Uint16 CreatePoint(
		std::vector<Triangle3D> & results, const Vector3D<> & position, const float radius);
	Uint16 CreateLine(
		std::vector<Triangle3D> & results, const Vector3D<> & startPoint,
		const Vector3D<> & endPoint, const float radius);
}

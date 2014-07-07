#pragma once

#include <Utilities/Algebra/Algebra3D.h>

#include <vector>

namespace utils {
	void MarchingCube(
		std::vector<Triangle3D> & resultTries,
		const float isoThreshold,
		const GridCell & grid);
}

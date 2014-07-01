#pragma once

#include <Utilities/Algebra/Algebra3.h>

#include <vector>

namespace utils {
	void MarchingCube(
		std::vector<Triangle> & resultTries,
		const float isoThreshold,
		const GridCell & grid);
}

#pragma once

#include <Utilities/DataStructures.h>

#include <vector>

namespace utils {
	void MarchingCube(
		std::vector<Triangle> & resultTries,
		const float isoThreshold,
		const GridCell & grid);
}

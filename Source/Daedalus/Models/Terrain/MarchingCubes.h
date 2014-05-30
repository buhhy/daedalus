#pragma once

#include "Engine.h"
#include "DataStructures.h"

namespace utils {
	void MarchingCube(
		TArray<Triangle> & resultTries,
		const float isoThreshold,
		const GridCell & grid);
}

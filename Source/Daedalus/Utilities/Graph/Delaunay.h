#pragma once

#include "Engine.h"
#include "GraphDatastructures.h"

#include <vector>

namespace utils {
	/**
	 * Basic divide and conquer algorithm and data structure taken from here:
	 * http://www.geom.uiuc.edu/~samuelp/del_project.html
	 */
	void BuildDelaunay2D(
		delaunay::DelaunayGraph & graph,
		const std::vector<Vector2<> > & inputPoints);
}

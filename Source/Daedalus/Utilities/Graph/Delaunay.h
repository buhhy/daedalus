#pragma once

#include "Engine.h"
#include "GraphDatastructures.h"

#include <vector>

namespace utils {
	typedef std::vector<std::pair<utils::Vector2<>, uint64> > InputVertexList;
	/**
	 * Basic divide and conquer algorithm and data structure taken from here:
	 * http://www.geom.uiuc.edu/~samuelp/del_project.html
	 */
	void BuildDelaunay2D(
		DelaunayGraph & graph,
		const InputVertexList & inputPoints);
}

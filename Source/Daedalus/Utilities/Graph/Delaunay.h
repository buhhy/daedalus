#pragma once

#include "Engine.h"
#include "GraphDatastructures.h"

#include <vector>

namespace utils {
	typedef std::vector<std::pair<utils::Vector2<>, uint64> > InputVertexList;
	/**
	 * Basic divide and conquer algorithm and data structure taken from here:
	 * http://www.geom.uiuc.edu/~samuelp/del_project.html. My implementation will handle
	 * merging tileable graphs without consolidating them into a single graph.
	 */
	void BuildDelaunay2D(
		DelaunayGraph & graph,
		const InputVertexList & inputPoints);

	void MergeDelaunayTileEdge(
		DelaunayGraph & leftGraph, DelaunayGraph & rightGraph,
		const uint64 lowerTangentLeft, const uint64 lowerTangentRight,
		const uint64 upperTangentLeft, const uint64 upperTangentRight);
}

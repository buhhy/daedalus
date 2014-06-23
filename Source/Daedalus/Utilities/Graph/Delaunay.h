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

	/**
	 * @param graphs An array containing the 4 corner Delaunay graphs in the following
	 *               order: top-left, top-right, bottom-left, bottom-right. The second
	 *               value in the pair contains the index of the point in the convex hull
	 *               that is used in the merging procedure.
	 */
	void MergeDelaunayTileCorner(std::array<std::pair<DelaunayGraph *, uint64>, 4> & graphs);
}

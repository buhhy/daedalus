#include "Daedalus.h"
#include "Delaunay.h"
#include "Constants.h"
#include "Vector3.h"

#include <algorithm>

namespace utils {
	using namespace delaunay;

	// Use tail recursion
	void Divide(
		DelaunayGraph & results,
		const uint64 start, const uint64 end
	) {
		// End condition when less than 4 vertices counted
		uint64 count = end - start + 1;
		if (count < 4) {
			if (count == 3) {
				// Triangle, need to find CW ordering
				Vector3<double> v1((results.Vertices[start])->Point, 0);
				Vector3<double> v2((results.Vertices[start + 1])->Point, 0);
				Vector3<double> v3((results.Vertices[start + 2])->Point, 0);
				Vector3<double> sub12 = v1 - v2;
				Vector3<double> sub32 = v3 - v2;

				if (sub12.Cross<double>(sub32).Z > 0) {
					// CW
					results.CreateTriangle(start, start + 1, start + 2);
				} else {
					// CCW
					results.CreateTriangle(start + 2, start + 1, start);
				}
			} else if (count == 2) {
				// Otherwise, just create a line
				results.CreateEdge(start, start + 1);
			}
		} else {
			uint64 half = (end + start) / 2;
			Divide(results, start, half);
			Divide(results, half + 1, end);
		}
	}

	DelaunayGraph BuildDelaunay2D(
		std::vector<double> & results,
		const std::vector<Vector2<double> *> & inputPoints
	) {
		std::vector<Vector2<double> *> copiedPoints = inputPoints;
		DelaunayGraph graph;

		// Sort by X from left to right, then Y from top down to resolve conflicts
		std::sort(
			copiedPoints.begin(),
			copiedPoints.end(),
			[] (const Vector2<double> * p1, const Vector2<double> * p2) {
				return *p1 < *p2;
			});

		for (auto it = copiedPoints.cbegin(); it != copiedPoints.cend(); it++)
			graph.CreateVertex(**it);

		// Run if at least 2 vertex
		if (graph.Vertices.size() > 1)
			Divide(graph, 0, graph.Vertices.size() - 1);

		return graph;
	}
}
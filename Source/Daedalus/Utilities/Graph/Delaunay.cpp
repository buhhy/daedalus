#include "Daedalus.h"
#include "Delaunay.h"
#include "Constants.h"
#include "DataStructures.h"

#include <algorithm>

namespace utils {
	namespace delaunay {
		/**
			* Returns 1: CW, 0: Colinear, -1: CCW
			*/
		double IsCWWinding(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			Vector2<> sub12 = v1->Point - v2->Point;
			Vector2<> sub32 = v3->Point - v2->Point;

			// All degenerate triangles count as CW triangles
			return sub12.Determinant(sub32);
		}

		float FindAngle(const Vector2<> & v1, const Vector2<> & v2) {
			float result = FMath::Atan2(v1.Determinant(v2), v1.Dot(v2));
			if (result < 0)
				result += 2 * M_PI;
			return result;
		}

		std::pair<Face *, int8> DelaunayGraph::AdjustNewFaceAdjacencies(
			Face * const newFace,
			const uint8 pivotIndex
		) {
			int8 pivotCWIndex = newFace->GetCWVertexIndex(pivotIndex);
			int8 pivotCCWIndex = newFace->GetCCWVertexIndex(pivotIndex);

			const auto pivotPoint = newFace->Vertices[pivotIndex];
			const auto pivotCWPoint = newFace->Vertices[pivotCWIndex];
			const auto pivotCCWPoint = newFace->Vertices[pivotCCWIndex];

			Face * otherFace = pivotPoint->IncidentFace;

			// If pivot point has no faces yet, assign the new one
			if (otherFace == NULL) {
				pivotPoint->IncidentFace = newFace;
				return std::make_pair((Face *) NULL, -1);
			}

			// We need to find the face immediately clockwise and counter-clockwise of the
			// new face to adjust adjacencies. The face to the immediate CW of the new face
			// will need a pointer to the new face while the new face will link to the
			// immediate CCW face.
			
			double CCWMinAngle = 10.0, CWMinAngle = 10.0;
			int8 CWVertexIndex = -1;
			Face * CCWFace = NULL, * CWFace = NULL;

			Vector2<> CCWCompareEdge = pivotCWPoint->Point - pivotPoint->Point;
			Vector2<> CWCompareEdge = pivotCCWPoint->Point - pivotPoint->Point;

			// Traverse faces currently at pivot point in CCW fashion, keeping the smallest
			// available CCW angle, as well as the smallest available CW angle.
			do {
				int8 otherPivotIndex = otherFace->FindVertex(pivotPoint);
				int8 otherPivotCWIndex = otherFace->GetCWVertexIndex(otherPivotIndex);
				int8 otherPivotCCWIndex = otherFace->GetCCWVertexIndex(otherPivotIndex);

				// Get the CCW point from the pivot from the other face
				const auto otherPivotCCW = otherFace->Vertices[otherPivotCCWIndex];
				// Get the CW point from the pivot from the other face
				const auto otherPivotCW = otherFace->Vertices[otherPivotCWIndex];

				// Compare the minimum CCW face with this current one to get new minimum
				if (*otherPivotCCW == *pivotCWPoint) {
					// If the other face shares an edge with the current face, then we can break
					CCWFace = otherFace;
					CCWMinAngle = -10;
				} else {
					auto angle = FindAngle(CCWCompareEdge, otherPivotCCW->Point - pivotPoint->Point);
					if (angle < CCWMinAngle) {
						CCWMinAngle = angle;
						CCWFace = otherFace;
					}
				}
				
				// Compare the minimum CW face with this current one to get new minimum
				// Do the same as above except for the CW rotation
				if (*otherPivotCW == *pivotCCWPoint) {
					CWFace = otherFace;
					CWMinAngle = -10;
					CWVertexIndex = otherPivotCCWIndex;
				} else {
					auto angle = FindAngle(otherPivotCW->Point - pivotPoint->Point, CWCompareEdge);
					if (angle < CWMinAngle) {
						CWMinAngle = angle;
						CWFace = otherFace;
						CWVertexIndex = otherPivotCCWIndex;
					}
				}

				otherFace = otherFace->GetAdjacentFaceCCW(pivotPoint);
			} while (otherFace != pivotPoint->IncidentFace);

			if (CCWFace != NULL) {
				// Insert current face into the adjacency loop single-linked-list style
				newFace->AdjacentFaces[pivotCCWIndex] = CCWFace;
			}
			
			if (CWFace != NULL) {
				// We can't assign CW face adjacency yet, since it could lead to a circular
				// dependency on the incomplete face.
				return std::make_pair(CWFace, CWVertexIndex);
			}
			return std::make_pair((Face *) NULL, -1);
		}

		Vertex * DelaunayGraph::AddVertex(Vertex * const vertex) {
			Vertices.insert(vertex);
			return vertex;
		}

		Face * DelaunayGraph::CreateFace(Vertex * const v1, Vertex * const v2) {
			Face * newFace = new Face(v1, v2);

			// Modify adjacencies
			std::array<std::pair<Face *, uint8>, 3> adjusts({
				AdjustNewFaceAdjacencies(newFace, 0),
				AdjustNewFaceAdjacencies(newFace, 1)
			});

			for (auto adjust : adjusts) {
				if (adjust.first != NULL)
					adjust.first->AdjacentFaces[adjust.second] = newFace;
			}
			
			Faces.insert(newFace);

			return newFace;
		}

		/**
			* Indices of vertices should be provided in CW winding.
			*/
		Face * DelaunayGraph::CreateFace(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			Vertex * inV1 = v1;
			Vertex * inV2 = v2;
			Vertex * inV3 = v3;

			// Insert face with clockwise vertex winding order
			if (IsCWWinding(v1, v2, v3) < 0) {
				inV2 = v3;
				inV3 = v2;
			}

			Face * newFace = new Face(inV1, inV2, inV3);

			// Modify adjacencies
			std::array<std::pair<Face *, uint8>, 3> adjusts({
				AdjustNewFaceAdjacencies(newFace, 0),
				AdjustNewFaceAdjacencies(newFace, 1),
				AdjustNewFaceAdjacencies(newFace, 2)
			});

			for (auto adjust : adjusts) {
				if (adjust.first != NULL)
					adjust.first->AdjacentFaces[adjust.second] = newFace;
			}

			Faces.insert(newFace);

			return newFace;
		}

		const std::vector<Vertex const *> DelaunayGraph::GetVertices() const {
			std::vector<Vertex const *> ret;
			for (auto it : Vertices) ret.push_back(it);
			return ret;
		}

		const std::vector<Face const *> DelaunayGraph::GetFaces() const {
			std::vector<Face const *> ret;
			for (auto it : Faces) ret.push_back(it);
			return ret;
		}

		const std::vector<Edge> DelaunayGraph::GetUniqueEdges() const {
			std::unordered_set<Edge> edgeSet;
			std::vector<Edge> ret;

			for (auto it : Faces) {
				edgeSet.insert({ it->Vertices[0], it->Vertices[1] });
				if (!it->IsDegenerate) {
					edgeSet.insert({ it->Vertices[0], it->Vertices[2] });
					edgeSet.insert({ it->Vertices[1], it->Vertices[2] });
				}
			}

			for (auto it : edgeSet) ret.push_back(it);

			return ret;
		}
	}

	using namespace delaunay;

	const double DOUBLE_MIN = std::numeric_limits<double>::min();
	const double DOUBLE_MAX = std::numeric_limits<double>::max();

	inline uint64 BoundedAdd(uint64 source, uint64 limit, uint64 amount = 1) {
		source += amount;
		if (source >= limit) return source - limit;
		return source;
	}

	inline uint64 BoundedSubtract(uint64 source, uint64 limit, uint64 amount = 1) {
		if (source < amount) return source + limit - amount;
		return source - amount;
	}

	typedef std::vector<Vertex *> ConvexHull;
	typedef std::pair<uint64, uint64> Tangent;

	void MergeDelaunay(
		DelaunayGraph & results,
		const std::vector<Vertex *> & sortedVertices,
		const uint64 start1,
		const uint64 end1,
		const uint64 start2,
		const uint64 end2,
		const Tangent & upperTangent,
		const Tangent & lowerTangent
	) {
		//bool left, right;

		//do {
		//	left = false, right = false;
		//} while (left || right);
	}

	uint64 ConvexHullLeftmostIndex(const ConvexHull & hull) {
		uint64 leftIndex = 0;
		double threshold = DOUBLE_MAX;
		for (uint64 i = 0u; i < hull.size(); i++) {
			if (hull[i]->Point.X < threshold) {
				leftIndex = i;
				threshold = hull[i]->Point.X;
			}
		}
		return leftIndex;
	}

	uint64 ConvexHullRightmostIndex(const ConvexHull & hull) {
		uint64 rightIndex = 0;
		double threshold = DOUBLE_MIN;
		for (uint64 i = 0u; i < hull.size(); i++) {
			if (hull[i]->Point.X > threshold) {
				rightIndex = i;
				threshold = hull[i]->Point.X;
			}
		}
		return rightIndex;
	}

	/**
	 * Retrieves the upper tangent of the provided left and right hulls.
	 */
	Tangent UpperTangent(
		const ConvexHull & leftHull,
		const ConvexHull & rightHull
	) {
		// Find the index of the highest X in leftHull and index of the lowest X in rightHull
		auto leftSize = leftHull.size();
		auto rightSize = rightHull.size();
		uint64 leftIndex = ConvexHullRightmostIndex(leftHull);
		uint64 rightIndex = ConvexHullLeftmostIndex(rightHull);
		uint64 nextLeft = BoundedSubtract(leftIndex, leftSize);
		uint64 nextRight = BoundedAdd(rightIndex, rightSize);
		
		bool done;

		do {
			done = true;

			while (
				IsCWWinding(leftHull[nextLeft], rightHull[rightIndex], leftHull[leftIndex]) > 0
			) {
				nextLeft = BoundedSubtract(nextLeft, leftSize);
				leftIndex = BoundedSubtract(leftIndex, leftSize);
			}

			while (
				IsCWWinding(rightHull[nextRight], rightHull[rightIndex], leftHull[leftIndex]) > 0
			) {
				nextRight = BoundedAdd(nextRight, rightSize);
				rightIndex = BoundedAdd(rightIndex, rightSize);
				done = false;
			}
		} while (!done);

		return std::make_pair(leftIndex, rightIndex);
	}

	/**
	 * Retrieves the lower tangent of the provided left and right hulls;
	 */
	Tangent LowerTangent(const ConvexHull & leftHull, const ConvexHull & rightHull) {
		// Find the index of the highest X in leftHull and index of the lowest X in rightHull
		auto leftSize = leftHull.size();
		auto rightSize = rightHull.size();
		uint64 leftIndex = ConvexHullRightmostIndex(leftHull);
		uint64 rightIndex = ConvexHullLeftmostIndex(rightHull);
		uint64 nextLeft = BoundedAdd(leftIndex, leftSize);
		uint64 nextRight = BoundedSubtract(rightIndex, rightSize);
		
		bool done;

		do {
			done = true;

			while (
				IsCWWinding(leftHull[leftIndex], rightHull[rightIndex], leftHull[nextLeft]) > 0
			) {
				nextLeft = BoundedAdd(nextLeft, leftSize);
				leftIndex = BoundedAdd(leftIndex, leftSize);
			}

			while (
				IsCWWinding(leftHull[leftIndex], rightHull[rightIndex], rightHull[nextRight]) > 0
			) {
				rightIndex = BoundedSubtract(rightIndex, rightSize);
				nextRight = BoundedSubtract(nextRight, rightSize);
				done = false;
			}
		} while (!done);

		return std::make_pair(leftIndex, rightIndex);
	}

	ConvexHull MergeConvexHulls(
		const ConvexHull & leftHull,
		const ConvexHull & rightHull,
		const Tangent & upperTangent,
		const Tangent & lowerTangent
	) {
		ConvexHull newConvexHull;

		uint64 count = (upperTangent.first - lowerTangent.first + leftHull.size()) % leftHull.size() + 1;
		for (uint64 i = lowerTangent.first, c = 0; c < count; c++, i++) {
			if (i >= leftHull.size()) i -= leftHull.size();
			newConvexHull.push_back(leftHull[i]);
		}
		
		count = (lowerTangent.second - upperTangent.second + rightHull.size()) % rightHull.size() + 1;
		for (uint64 i = upperTangent.second, c = 0; c < count; c++, i++) {
			if (i >= rightHull.size()) i -= rightHull.size();
			newConvexHull.push_back(rightHull[i]);
		}

		return newConvexHull;
	}

	// Return a CW array of vertices representing the convex hull
	ConvexHull Divide(
		DelaunayGraph & results,
		const std::vector<Vertex *> & sortedVertices,
		const uint64 start, const uint64 end
	) {
		// End condition when less than 4 vertices counted
		uint64 count = end - start + 1;
		if (count < 4) {
			ConvexHull hull;
			if (count == 3) {
				// Create triangle with 3 vertices
				auto newFace = results.CreateFace(
					sortedVertices[start],
					sortedVertices[start + 1],
					sortedVertices[start + 2]);

				for (uint8 i = 0u; i < newFace->NumVertices; i++)
					hull.push_back(newFace->Vertices[i]);
			} else if (count == 2) {
				// Otherwise, just create a line
				auto newFace = results.CreateFace(
					sortedVertices[start],
					sortedVertices[start + 1]);

				for (uint8 i = 0u; i < newFace->NumVertices; i++)
					hull.push_back(newFace->Vertices[i]);
			} else {
				// This shouldn't ever happen
				hull.push_back(sortedVertices[start]);
			}

			return hull;
		} else {
			uint64 half = (end + start) / 2;
			auto leftHull = Divide(results, sortedVertices, start, half);
			auto rightHull = Divide(results, sortedVertices, half + 1, end);
			auto upperTangent = UpperTangent(leftHull, rightHull);
			auto lowerTangent = LowerTangent(leftHull, rightHull);
			MergeDelaunay(
				results, sortedVertices, start, half, half + 1, end,
				upperTangent, lowerTangent);
			return MergeConvexHulls(leftHull, rightHull, upperTangent, lowerTangent);
		}
	}

	void Test(DelaunayGraph & graph) {
		std::vector<Vector2<> > testPoints;
		std::vector<Vertex *> testVertices;
		testPoints.push_back({ 0.1, 0.2 });
		testPoints.push_back({ 0.2, 0.3 });
		testPoints.push_back({ 0.2, 0.1 });
		testPoints.push_back({ 0.3, 0.2 });
		testPoints.push_back({ 0.3, 0.4 });
		testPoints.push_back({ 0.5, 0.9 });
		testPoints.push_back({ 0.8, 0.3 });
		testPoints.push_back({ 0.6, 0.7 });
		//testPoints.push_back({ 0.7, 0.2 });
		//testPoints.push_back({ 0.8, 0.1 });

		for (auto i = 0u; i < testPoints.size(); i++)
			testVertices.push_back(new Vertex(testPoints[i], i));

		std::sort(
			testVertices.begin(),
			testVertices.end(),
			[] (Vertex * const p1, Vertex * const p2) {
				return p1->Point < p2->Point;
			});

		for (auto i = 0u; i < testPoints.size(); i++)
			graph.AddVertex(testVertices[i]);

		graph.ConvexHull = Divide(graph, testVertices, 0, graph.VertexCount() - 1);

		//graph.CreateFace(testVertices[0], testVertices[1]);
		//graph.CreateFace(testVertices[3], testVertices[1], testVertices[2]);

		//graph.CreateFace(testVertices[0], testVertices[1], testVertices[2]);
		//graph.CreateFace(testVertices[3], testVertices[1], testVertices[2]);
		//graph.CreateFace(testVertices[1], testVertices[3], testVertices[4]);
	}

	void BuildDelaunay2D(DelaunayGraph & graph, const std::vector<Vector2<> > & inputPoints) {
		std::vector<Vertex *> copiedVertices;

		for (auto i = 0u; i < inputPoints.size(); i++)
			copiedVertices.push_back(new Vertex(inputPoints[i], i));

		// Sort by X from left to right, then Y from top down to resolve conflicts
		std::sort(
			copiedVertices.begin(),
			copiedVertices.end(),
			[] (Vertex * const p1, Vertex * const p2) {
				return p1->Point < p2->Point;
			});

		for (auto it = copiedVertices.cbegin(); it != copiedVertices.cend(); it++)
			graph.AddVertex(*it);

		// Run if at least 2 vertex
		if (graph.VertexCount() > 1)
			graph.ConvexHull = Divide(graph, copiedVertices, 0, graph.VertexCount() - 1);
		//Test(graph);
	}
}

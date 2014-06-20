#include "Daedalus.h"
#include "Delaunay.h"
#include "Constants.h"
#include "DataStructures.h"

#include <algorithm>

namespace utils {
	using namespace delaunay;

	typedef std::pair<uint64, uint64> Tangent;

	void MergeDelaunay(
		DelaunayGraph & leftGraph,
		DelaunayGraph & rightGraph,
		const ConvexHull & leftHull,
		const ConvexHull & rightHull,
		const Tangent & upperTangent,
		const Tangent & lowerTangent
	) {
		const bool isSame = &leftGraph == &rightGraph;
		bool foundLeft, foundRight;
		bool takeLeft, takeRight;
		bool isLeftDone = false, isRightDone = false;

		const Vector2<int64> rightOffset = rightGraph.GraphOffset() - leftGraph.GraphOffset();

		// If the left and right graphs are different, we need to the add an offset to the
		// right graph since coordinates are local to the tile, and not absolute.
		const auto addRightOffset = [&] (const Vector2<> & input) -> Vector2<> {
			return isSame ? input : Vector2<>(rightOffset.X + input.X, rightOffset.Y + input.Y);
		};

		// Ditto for the left graph, however we use the left tile as (0, 0) for simplicity,
		// so this is only useful when adding ghost vertices for the right tile.
		const auto addLeftOffset = [&] (const Vector2<> & input) -> Vector2<> {
			return isSame ? input : Vector2<>(-rightOffset.X + input.X, -rightOffset.Y + input.Y);
		};

		std::vector<std::array<Vertex *, 3> > leftAddedFaces;
		std::vector<std::array<Vertex *, 3> > rightAddedFaces;
		std::deque<Vertex *> leftVertexQueue;
		std::deque<Vertex *> rightVertexQueue;

		leftHull.GetSequenceCCW(
			leftVertexQueue, lowerTangent.first, upperTangent.first);
		rightHull.GetSequenceCW(
			rightVertexQueue, lowerTangent.second, upperTangent.second);

		Vertex * baseLeft = leftVertexQueue.front();
		Vertex * baseRight = rightVertexQueue.front();

		leftVertexQueue.pop_front();
		rightVertexQueue.pop_front();
		
		Vertex * leftCandidate = NULL, * rightCandidate = NULL, * nextCandidate = NULL;
		Face * faceIt = NULL;

		do {
			foundLeft = false, foundRight = false;

			if (isLeftDone) {
				// If left side is finished, there is no need to change the left candidate,
				// it will always be the last one used
				foundLeft = false;
			} else {
				// Get left candidate
				leftCandidate = leftVertexQueue.front();
				faceIt = leftGraph.FindFace(leftCandidate, baseLeft);

				if (faceIt == NULL || faceIt->VertexCount() < 3) {
					foundLeft = FindWinding(
						addRightOffset(baseRight->GetPoint()),
						baseLeft->GetPoint(),
						leftCandidate->GetPoint()) > 0;
				} else {
					do {
						// Only accept candidates that form angles < 180
						if (FindWinding(
								addRightOffset(baseRight->GetPoint()),
								baseLeft->GetPoint(),
								leftCandidate->GetPoint()) <= 0)
							break;
						nextCandidate = faceIt->GetCCWVertex(leftCandidate);
						// If there is no next candidate, it must mean that there was no immediate face
						// counter-clockwise from the current face. Thus there is at least a 180 deg
						// angle to next possible candidate.
						if (nextCandidate == NULL) {
							foundLeft = true;
							break;
						}
						auto circumcircle = CalculateCircumcircle(
							leftCandidate->GetPoint(),
							addRightOffset(baseRight->GetPoint()),
							baseLeft->GetPoint());
						// For ambiguous points, don't delete
						if (IsWithinCircumcircle(nextCandidate->GetPoint(), circumcircle) > 0) {
							// If within circumcircle, we need to delete edge
							auto faceNext = faceIt->GetAdjacentFaceCCW(baseLeft);
							if (faceIt == faceNext) {
								// This is the only face attached to the left base vertex
								leftGraph.RemoveFace(faceIt);
								faceIt = NULL;
								foundLeft = true;
							} else {
								leftGraph.RemoveFace(faceIt);
								faceIt = faceNext;
							}
							leftVertexQueue.push_front(nextCandidate);
							leftCandidate = nextCandidate;
						} else {
							// Otherwise, this point is valid, and should be submitted as candidate
							foundLeft = true;
						}
					} while (faceIt != NULL && !foundLeft);
				}
			}

			if (isRightDone) {
				// If right side is finished, there is no need to change the right candidate,
				// it will always be the last one used
				foundRight = false;
			} else {
				// Get right candidate
				rightCandidate = rightVertexQueue.front();
				faceIt = leftGraph.FindFace(rightCandidate, baseRight);

				if (faceIt == NULL || faceIt->VertexCount() < 3) {
					foundRight = FindWinding(
						addRightOffset(rightCandidate->GetPoint()),
						addRightOffset(baseRight->GetPoint()),
						baseLeft->GetPoint()) > 0;
				} else {
					do {
						// Only accept candidates that form angles < 180
						if (FindWinding(
								addRightOffset(rightCandidate->GetPoint()),
								addRightOffset(baseRight->GetPoint()),
								baseLeft->GetPoint()) <= 0)
							break;
						nextCandidate = faceIt->GetCWVertex(rightCandidate);
						// If there is no next candidate, it must mean that there was no immediate face
						// clockwise from the current face. Thus there is at least a 180 deg angle to
						// next possible candidate.
						if (nextCandidate == NULL) {
							foundRight = true;
							break;
						}
						auto circumcircle = CalculateCircumcircle(
							addRightOffset(rightCandidate->GetPoint()),
							addRightOffset(baseRight->GetPoint()),
							baseLeft->GetPoint());
						// For ambiguous points, don't delete
						if (IsWithinCircumcircle(
								addRightOffset(nextCandidate->GetPoint()), circumcircle) > 0) {
							// If within circumcircle, we need to delete edge
							auto faceNext = faceIt->GetAdjacentFaceCW(baseRight);
							if (faceIt == faceNext) {
								// This is the only face attached to the right base vertex
								leftGraph.RemoveFace(faceIt);
								faceIt = NULL;
								foundRight = true;
							} else {
								leftGraph.RemoveFace(faceIt);
								faceIt = faceNext;
							}
							rightVertexQueue.push_front(nextCandidate);
							rightCandidate = nextCandidate;
						} else {
							// Otherwise, this point is valid, and should be submitted as candidate
							foundRight = true;
						}
					} while (faceIt != NULL && !foundRight);
				}
			}

			takeLeft = foundLeft; takeRight = foundRight;

			if (takeLeft && takeRight) {
				// If 2 potential candidates, select one of the 2 candidates, left first
				auto circumcircle = CalculateCircumcircle(
					leftCandidate->GetPoint(),
					addRightOffset(baseRight->GetPoint()),
					baseLeft->GetPoint());

				if (foundLeft && IsWithinCircumcircle(
						addRightOffset(rightCandidate->GetPoint()), circumcircle) > 0) {
					// If the right candidate is within the left circumcircle, right is selected
					takeLeft = false;
				} else {
					// Otherwise, left candidate is selected
					takeRight = false;
				}
			}

			if (takeRight) {
				rightAddedFaces.push_back({{ rightCandidate, baseRight, baseLeft }});
				rightVertexQueue.pop_front();
				baseRight = rightCandidate;
			} else if (takeLeft) {
				leftAddedFaces.push_back({{ leftCandidate, baseRight, baseLeft }});
				leftVertexQueue.pop_front();
				baseLeft = leftCandidate;
			}

			isLeftDone = leftVertexQueue.empty();
			isRightDone = rightVertexQueue.empty();
		} while ((takeLeft || takeRight) && (!isLeftDone || !isRightDone));

		for (auto f : leftAddedFaces)
			leftGraph.AddFace(f[0], f[1], f[2]);
		for (auto f : rightAddedFaces)
			rightGraph.AddFace(f[0], f[1], f[2]);
	}

	/**
	 * Abstracts out the tangent finding algorithm since finding the top and bottom tangents
	 * used much of the same code.
	 * @param nextLeftIndex This function retrieves the next face index for the left hull, for
	 *                      finding the top tangent, this means traversing the convex hull in
	 *                      a CCW manner to find the top-right-most tangent vertex.
	 * @param nextRightIndex This function is same as the above, but for the right hull, meaning
	 *                       it should provide the opposite result to the next left index.
	 * @param getWinding This function should get the triangle winding of the 3 provided
	 *                   vertices: the next vertex on either side, the left vertex and the
	 *                   right vertex. This function should return -1 when the next vertex is
	 *                   on the inside of the tangent created by the left and right vertices,
	 *                   0 for collinear points and 1 when the next vertex is outside of the
	 *                   tangent line.
	 */
	Tangent FindTangent(
		const ConvexHull & leftHull,
		const ConvexHull & rightHull,
		const std::function<uint64 (const ConvexHull &, uint64)> & nextLeftIndex,
		const std::function<uint64 (const ConvexHull &, uint64)> & nextRightIndex,
		const std::function<int8 (Vertex * const, Vertex * const, Vertex * const)> & getWinding
	) {
		// Find the index of the highest X in leftHull and index of the lowest X in rightHull
		uint64 leftIndex = leftHull.RightVertexIndex();
		uint64 rightIndex = rightHull.LeftVertexIndex();
		uint64 nextLeft = nextLeftIndex(leftHull, leftIndex);
		uint64 nextRight = nextRightIndex(rightHull, rightIndex);
		
		bool done;

		do {
			done = true;
			
			// Loop until the next left vertex is below the tangent
			while (true) {
				int8 winding = getWinding(
					leftHull[nextLeft], leftHull[leftIndex], rightHull[rightIndex]);

				// If the next left is below the tangent, then we've reached the apex
				if (winding < 0) {
					break;
				} else if (winding == 0) {
					// In the case of collinear left, next left, and right points, select
					// the point with the shortest distance
					double newDist = (leftHull[nextLeft]->GetPoint() -
						rightHull[rightIndex]->GetPoint()).Length2();
					double oldDist = (leftHull[leftIndex]->GetPoint() -
						rightHull[rightIndex]->GetPoint()).Length2();
					if (newDist >= oldDist)
						break;
				}
				nextLeft = nextLeftIndex(leftHull, nextLeft);
				leftIndex = nextLeftIndex(leftHull, leftIndex);
			}
			
			// Loop until the next right vertex is below the tangent
			while (true) {
				int8 winding = getWinding(
					rightHull[nextRight], leftHull[leftIndex], rightHull[rightIndex]);

				// If the next right is below the tangent, then we've reached the apex
				if (winding < 0) {
					break;
				} else if (winding == 0) {
					// In the case of collinear left, right, and next right points, select
					// the point with the shortest distance
					double newDist = (rightHull[nextRight]->GetPoint() -
						leftHull[leftIndex]->GetPoint()).Length2();
					double oldDist = (rightHull[rightIndex]->GetPoint() -
						leftHull[leftIndex]->GetPoint()).Length2();
					if (newDist >= oldDist)
						break;
				}
				nextRight = nextRightIndex(rightHull, nextRight);
				rightIndex = nextRightIndex(rightHull, rightIndex);
				done = false;
			}
		} while (!done);

		return std::make_pair(leftIndex, rightIndex);
	}

	/**
	 * Retrieves the upper tangent of the provided left and right hulls.
	 */
	Tangent UpperTangent(const ConvexHull & leftHull, const ConvexHull & rightHull) {
		// Find the index of the highest X in leftHull and index of the lowest X in rightHull
		return FindTangent(
			leftHull, rightHull,
			[] (const ConvexHull & leftHull, const uint64 index) {
				return leftHull.PrevIndex(index);
			},
			[] (const ConvexHull & rightHull, const uint64 index) {
				return rightHull.NextIndex(index);
			},
			[] (Vertex * const nextVert, Vertex * const leftVert, Vertex * const rightVert) {
				return IsCWWinding(nextVert, rightVert, leftVert);
			});
	}

	/**
	 * Retrieves the lower tangent of the provided left and right hulls;
	 */
	Tangent LowerTangent(const ConvexHull & leftHull, const ConvexHull & rightHull) {
		// Find the index of the highest X in leftHull and index of the lowest X in rightHull
		return FindTangent(
			leftHull, rightHull,
			[] (const ConvexHull & leftHull, const uint64 index) {
				return leftHull.NextIndex(index);
			},
			[] (const ConvexHull & rightHull, const uint64 index) {
				return rightHull.PrevIndex(index);
			},
			[] (Vertex * const nextVert, Vertex * const leftVert, Vertex * const rightVert) {
				return IsCWWinding(leftVert, rightVert, nextVert);
			});
	}

	ConvexHull MergeConvexHulls(
		const ConvexHull & leftHull,
		const ConvexHull & rightHull,
		const Tangent & upperTangent,
		const Tangent & lowerTangent
	) {
		ConvexHull newConvexHull;

		uint64 count = (upperTangent.first - lowerTangent.first + leftHull.Size()) %
			leftHull.Size() + 1;
		for (uint64 i = lowerTangent.first, c = 0; c < count; c++, i++) {
			if (i >= leftHull.Size()) i -= leftHull.Size();
			newConvexHull.AddVertex(leftHull[i]);
		}
		
		count = (lowerTangent.second - upperTangent.second + rightHull.Size()) %
			rightHull.Size() + 1;
		for (uint64 i = upperTangent.second, c = 0; c < count; c++, i++) {
			if (i >= rightHull.Size()) i -= rightHull.Size();
			newConvexHull.AddVertex(rightHull[i]);
		}

		return newConvexHull;
	}

	// Return a CW array of vertices representing the convex hull
	ConvexHull Divide(
		DelaunayGraph & results,
		const std::vector<Vertex *> & sortedVertices,
		const uint64 start, const uint64 end,
		const uint64 minSubdivisionDepth,
		const uint64 subdivisionDepth = 0
	) {
		// End condition when less than 4 vertices counted
		uint64 count = end - start + 1;
		if (count < 4) {
			ConvexHull hull;
			if (count == 3) {
				// Create triangle with 3 vertices
				auto newFace = results.AddFace(
					sortedVertices[start],
					sortedVertices[start + 1],
					sortedVertices[start + 2]);
				
				if (newFace != NULL) {
					for (uint8 i = 0u; i < newFace->VertexCount(); i++)
						hull.AddVertex(newFace->Vertices[i]);
				} else {
					// The 3 points are colinear, add 2 edges instead
					results.AddFace(
						sortedVertices[start],
						sortedVertices[start + 1]);
					results.AddFace(
						sortedVertices[start + 1],
						sortedVertices[start + 2]);
					for (uint8 i = 0u; i < 3; i++)
						hull.AddVertex(sortedVertices[start + i]);
				}
			} else if (count == 2) {
				// Otherwise, just create a line
				auto newFace = results.AddFace(
					sortedVertices[start],
					sortedVertices[start + 1]);

				for (uint8 i = 0u; i < newFace->VertexCount(); i++)
					hull.AddVertex(newFace->Vertices[i]);
			} else {
				// This shouldn't ever happen
				hull.AddVertex(sortedVertices[start]);
			}

			return hull;
		} else {
			uint64 half = (end + start) / 2;
			auto leftHull = Divide(
				results, sortedVertices, start, half,
				minSubdivisionDepth, subdivisionDepth + 1);
			auto rightHull = Divide(
				results, sortedVertices, half + 1, end,
				minSubdivisionDepth, subdivisionDepth + 1);
			auto upperTangent = UpperTangent(leftHull, rightHull);
			auto lowerTangent = LowerTangent(leftHull, rightHull);

			if (subdivisionDepth >= minSubdivisionDepth) {
				MergeDelaunay(
					results, results,
					leftHull, rightHull,
					upperTangent, lowerTangent);
			}
			return MergeConvexHulls(leftHull, rightHull, upperTangent, lowerTangent);
		}
	}

	//void Test(DelaunayGraph & graph) {
	//	std::vector<Vector2<> > testPoints;
	//	std::vector<Vertex *> testVertices;
		//testPoints.push_back({ 0.1, 0.2 });
		//testPoints.push_back({ 0.2, 0.1 });
		//testPoints.push_back({ 0.2, 0.3 });
		//testPoints.push_back({ 0.2, 0.4 });
		//testPoints.push_back({ 0.3, 0.2 });
		//testPoints.push_back({ 0.4, 0.4 });
		//testPoints.push_back({ 0.5, 0.3 });
		//testPoints.push_back({ 0.6, 0.4 });
		//testPoints.push_back({ 0.6, 0.2 });
		//testPoints.push_back({ 0.6, 0.1 });

		
	//	testPoints.push_back({ 0.1, 0.1 });
	//	testPoints.push_back({ 0.1, 0.2 });
	//	testPoints.push_back({ 0.1, 0.3 });
	//	testPoints.push_back({ 0.2, 0.1 });
	//	testPoints.push_back({ 0.3, 0.1 });

	//	for (auto i = 0u; i < testPoints.size(); i++)
	//		testVertices.push_back(new Vertex(testPoints[i], i));

	//	std::sort(
	//		testVertices.begin(),
	//		testVertices.end(),
	//		[] (Vertex * const p1, Vertex * const p2) {
	//			return p1->GetPoint() < p2->GetPoint();
	//		});

	//	for (auto i = 0u; i < testPoints.size(); i++)
	//		graph.AddVertex(testVertices[i]);

	//	graph.ConvexHull = Divide(graph, testVertices, 0, graph.VertexCount() - 1, 0);
	//}

	void BuildDelaunay2D(DelaunayGraph & graph, const InputVertexList & inputPoints) {
		std::vector<Vertex *> copiedVertices;

		for (auto i = 0u; i < inputPoints.size(); i++)
			copiedVertices.push_back(graph.AddVertex(inputPoints[i].first, inputPoints[i].second));

		// Sort by X from left to right, then Y from top down to resolve conflicts
		std::sort(
			copiedVertices.begin(),
			copiedVertices.end(),
			[] (Vertex * const p1, Vertex * const p2) {
				return p1->GetPoint() < p2->GetPoint();
			});

		// Run if at least 2 vertex
		if (graph.VertexCount() > 1)
			graph.ConvexHull = Divide(graph, copiedVertices, 0, graph.VertexCount() - 1, 0);
		//Test(graph);
	}

	void MergeDelaunayTileEdge(
		DelaunayGraph & leftGraph, DelaunayGraph & rightGraph,
		const uint64 lowerTangentLeft, const uint64 lowerTangentRight,
		const uint64 upperTangentLeft, const uint64 upperTangentRight
	) {
		MergeDelaunay(
			leftGraph, rightGraph,
			leftGraph.ConvexHull, rightGraph.ConvexHull,
			Tangent(upperTangentLeft, upperTangentRight),
			Tangent(lowerTangentLeft, lowerTangentRight));
	}
}

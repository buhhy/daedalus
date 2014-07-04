#include <Daedalus.h>
#include <Utilities/DataStructures.h>
#include <Utilities/Constants.h>
#include "Delaunay.h"

#include <cassert>
#include <algorithm>

namespace utils {
	using namespace delaunay;
	using InputPointList = DelaunayBuilderDAC2D::InputPointList;
	using GraphHullIndexArray = DelaunayBuilderDAC2D::GraphHullIndexArray;
	using Tangent = DelaunayBuilderDAC2D::Tangent;
	using VertexComparator = DelaunayBuilderDAC2D::VertexComparator;
	using NextIndexFunction = DelaunayBuilderDAC2D::NextIndexFunction;
	using FindWindingFunction = DelaunayBuilderDAC2D::FindWindingFunction;
	using AddedFaceList = DelaunayBuilderDAC2D::AddedFaceList;

	const VertexComparator HorizontalVertexComparator =
		[] (Vertex * const v1, Vertex * const v2) {
			const auto & p1 = v1->GetPoint(), & p2 = v2->GetPoint();
			auto x = p2.X - p1.X;
			return std::abs(x) < FLOAT_ERROR ? p1.Y < p2.Y : x > 0;
		};

	const VertexComparator VerticalVertexComparator =
		[] (Vertex * const v1, Vertex * const v2) {
			const auto & p1 = v1->GetPoint(), & p2 = v2->GetPoint();
			auto y = p2.Y - p1.Y;
			return std::abs(y) < FLOAT_ERROR ? p1.X < p2.X : y > 0;
		};

	void DelaunayBuilderDAC2D::MergeDelaunay(
		DelaunayGraph & leftGraph,
		DelaunayGraph & rightGraph,
		const ConvexHull & leftHull,
		const ConvexHull & rightHull,
		const Tangent & upperTangent,
		const Tangent & lowerTangent
	) const {
		const bool isSame = &leftGraph == &rightGraph;
		bool foundLeft, foundRight;
		bool takeLeft, takeRight;
		bool isLeftDone = false, isRightDone = false;

		const Vector2<Int64> rightOffset = rightGraph.GraphOffset() - leftGraph.GraphOffset();

		// If the left and right graphs are different, we need to the add an offset to the
		// right graph since coordinates are local to the tile, and not absolute.
		const auto addRightOffset = [&] (const Vector2<> & input) -> Vector2<> {
			return isSame ? input : Vector2<>(rightOffset.X + input.X, rightOffset.Y + input.Y);
		};

		AddedFaceList leftAddedFaces;
		AddedFaceList rightAddedFaces;
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
			// DEBUG POINT
			if (Debugger) {
				Debugger->MergeStep(
					leftGraph, rightGraph, leftHull, rightHull,
					upperTangent, lowerTangent,
					leftAddedFaces, rightAddedFaces);
			}

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
				faceIt = rightGraph.FindFace(rightCandidate, baseRight);

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
								rightGraph.RemoveFace(faceIt);
								faceIt = NULL;
								foundRight = true;
							} else {
								rightGraph.RemoveFace(faceIt);
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

		for (auto & f : leftAddedFaces)
			leftGraph.AddFace(f[0], f[1], f[2]);
		for (auto & f : rightAddedFaces)
			rightGraph.AddFace(f[0], f[1], f[2]);
	}

	Tangent DelaunayBuilderDAC2D::FindTangent(
		const ConvexHull & leftHull,
		const ConvexHull & rightHull,
		const NextIndexFunction & nextLeftIndex,
		const NextIndexFunction & nextRightIndex,
		const FindWindingFunction & getWinding
	) const {
		// Find the index of the closest vertex in the left hull to the centroid of the right
		// hull, and do the same, but inverse for the right hull. This is a good starting point
		// because these 2 vertices are guaranteed to form a non-intersecting edge.
		Uint64 leftIndex = leftHull.ClosestVertexIndex(rightHull.Centroid());
		Uint64 rightIndex = rightHull.ClosestVertexIndex(leftHull.Centroid());
		Uint64 nextLeft = nextLeftIndex(leftHull, leftIndex);
		Uint64 nextRight = nextRightIndex(rightHull, rightIndex);
		
		bool done;

		do {
			done = true;
			
			// Loop until the next left vertex is below the tangent
			while (true) {
				Int8 winding = getWinding(
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
				Int8 winding = getWinding(
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

	Tangent DelaunayBuilderDAC2D::UpperTangent(
		const ConvexHull & leftHull,
		const ConvexHull & rightHull
	) const {
		// Find the index of the highest X in leftHull and index of the lowest X in rightHull
		return FindTangent(
			leftHull, rightHull,
			[] (const ConvexHull & leftHull, const Uint64 index) {
				return leftHull.PrevIndex(index);
			},
			[] (const ConvexHull & rightHull, const Uint64 index) {
				return rightHull.NextIndex(index);
			},
			[] (Vertex * const nextVert, Vertex * const leftVert, Vertex * const rightVert) {
				return IsCWWinding(nextVert, rightVert, leftVert);
			});
	}

	Tangent DelaunayBuilderDAC2D::LowerTangent(
		const ConvexHull & leftHull,
		const ConvexHull & rightHull
	) const {
		// Find the index of the highest X in leftHull and index of the lowest X in rightHull
		return FindTangent(
			leftHull, rightHull,
			[] (const ConvexHull & leftHull, const Uint64 index) {
				return leftHull.NextIndex(index);
			},
			[] (const ConvexHull & rightHull, const Uint64 index) {
				return rightHull.PrevIndex(index);
			},
			[] (Vertex * const nextVert, Vertex * const leftVert, Vertex * const rightVert) {
				return IsCWWinding(leftVert, rightVert, nextVert);
			});
	}

	ConvexHull DelaunayBuilderDAC2D::MergeConvexHulls(
		const ConvexHull & leftHull,
		const ConvexHull & rightHull,
		const Tangent & upperTangent,
		const Tangent & lowerTangent
	) const {
		ConvexHull newConvexHull;

		Uint64 count = (upperTangent.first - lowerTangent.first + leftHull.Size()) %
			leftHull.Size() + 1;
		for (Uint64 i = lowerTangent.first, c = 0; c < count; c++, i++) {
			if (i >= leftHull.Size()) i -= leftHull.Size();
			newConvexHull.AddVertex(leftHull[i]);
		}
		
		count = (lowerTangent.second - upperTangent.second + rightHull.Size()) %
			rightHull.Size() + 1;
		for (Uint64 i = upperTangent.second, c = 0; c < count; c++, i++) {
			if (i >= rightHull.Size()) i -= rightHull.Size();
			newConvexHull.AddVertex(rightHull[i]);
		}

		return newConvexHull;
	}

	void DelaunayBuilderDAC2D::DivideVertexList(
		std::vector<Vertex *> & leftHalf, std::vector<Vertex *> & rightHalf,
		std::vector<Vertex *> & vertices,
		const VertexComparator & comparator
	) const {
		std::sort(vertices.begin(), vertices.end(), comparator);
		size_t half = vertices.size() / 2;
		for (size_t i = 0; i < half; i++)
			leftHalf.push_back(vertices[i]);
		for (size_t i = half; i < vertices.size(); i++)
			rightHalf.push_back(vertices[i]);
	}

	ConvexHull DelaunayBuilderDAC2D::Divide(
		DelaunayGraph & results,
		std::vector<Vertex *> & vertices,
		const Uint32 currentSubdivisionDepth = 0
	) const {
		// End condition when less than 4 vertices counted
		Uint64 count = vertices.size();
		if (count < 4) {
			ConvexHull hull;
			if (count == 3) {
				// Create triangle with 3 vertices
				auto newFace = results.AddFace(vertices[0], vertices[1], vertices[2]);
				
				if (newFace != NULL) {
					for (Uint8 i = 0u; i < newFace->VertexCount(); i++)
						hull.AddVertex(newFace->Vertices[i]);
				} else {
					// The 3 points are colinear, add 2 edges instead
					results.AddFace(vertices[0], vertices[1]);
					results.AddFace(vertices[1], vertices[2]);

					for (Uint8 i = 0u; i < 3; i++)
						hull.AddVertex(vertices[i]);
				}
			} else if (count == 2) {
				// Otherwise, just create a line
				auto newFace = results.AddFace(
					vertices[0],
					vertices[1]);

				for (Uint8 i = 0u; i < newFace->VertexCount(); i++)
					hull.AddVertex(newFace->Vertices[i]);
			} else {
				assert(!"Divide: we should never divide to the point where there are less than 2 vertices");
				// This shouldn't ever happen
				hull.AddVertex(vertices[0]);
			}

			return hull;
		} else {
			const bool isHorizontal = currentSubdivisionDepth % 2 == 0;
			std::vector<Vertex *> leftHalf, rightHalf;
			DivideVertexList(leftHalf, rightHalf, vertices,
				(isHorizontal ? HorizontalVertexComparator : VerticalVertexComparator));
			auto leftHull = Divide(
				results, leftHalf, currentSubdivisionDepth + 1);
			auto rightHull = Divide(
				results, rightHalf, currentSubdivisionDepth + 1);
			auto upperTangent = UpperTangent(leftHull, rightHull);
			auto lowerTangent = LowerTangent(leftHull, rightHull);

			if (currentSubdivisionDepth >= SubdivisionDepthCap) {
				MergeDelaunay(
					results, results,
					leftHull, rightHull,
					upperTangent, lowerTangent);
			}
			return MergeConvexHulls(leftHull, rightHull, upperTangent, lowerTangent);
		}
	}

	/**
	 * Public
	 */

	void DelaunayBuilderDAC2D::BuildDelaunayGraph(
		DelaunayGraph & graph,
		const InputPointList & inputPoints
	) const {
		std::vector<Vertex *> copiedVertices;

		for (auto i = 0u; i < inputPoints.size(); i++)
			copiedVertices.push_back(
				graph.AddVertex(inputPoints[i].first, inputPoints[i].second));

		// Run if at least 2 vertex
		if (graph.VertexCount() > 1)
			graph.ConvexHull = Divide(graph, copiedVertices);
		//Test(graph);
	}

	void DelaunayBuilderDAC2D::MergeDelaunayTileEdge(
		DelaunayGraph & leftGraph, DelaunayGraph & rightGraph,
		const Uint32 lowerTangentLeft, const Uint32 lowerTangentRight,
		const Uint32 upperTangentLeft, const Uint32 upperTangentRight
	) const {
		MergeDelaunay(
			leftGraph, rightGraph,
			leftGraph.ConvexHull, rightGraph.ConvexHull,
			Tangent(upperTangentLeft, upperTangentRight),
			Tangent(lowerTangentLeft, lowerTangentRight));
	}

	void DelaunayBuilderDAC2D::MergeDelaunayTileCorner(GraphHullIndexArray & graphs) const {
		const Uint8 size = 4;
		std::array<utils::Vector2<>, size> points;
		std::array<Vertex *, size> vertices;

		// Get all 4 points with offsets to the top-left graph
		for (Uint8 i = 0; i < size; i++) {
			auto & graph = graphs[i];
			auto offset = graph.first->GraphOffset() - graphs[0].first->GraphOffset();
			auto vertex = graph.first->ConvexHull[graph.second];
			vertices[i] = vertex;
			points[i] = vertex->GetPoint() + offset.Cast<double>();
		}

		// Determine which cross-edge to use by checking for collinearity and circumcircles
		Uint8 p1, p2, p3, p4;
		auto circumcircle1 = utils::CalculateCircumcircle(
			points[0], points[1], points[2]);
		auto circumcircle2 = utils::CalculateCircumcircle(
			points[3], points[2], points[1]);
		if (utils::IsWithinCircumcircle(points[3], circumcircle1) == 1 ||
				utils::IsWithinCircumcircle(points[0], circumcircle2) == 1) {
			p1 = 1; p2 = 3; p3 = 0; p4 = 2;
		} else {
			p1 = 0; p2 = 1; p3 = 2; p4 = 3;
		}

		// Add these 2 filler faces to every size
		for (Uint8 i = 0; i < size; i++) {
			auto & graph = graphs[i].first;
			graph->AddFace(vertices[p1], vertices[p2], vertices[p3]);
			graph->AddFace(vertices[p4], vertices[p3], vertices[p2]);
		}

		// TODO: implement edge flipping to ensure correctness
	}
}

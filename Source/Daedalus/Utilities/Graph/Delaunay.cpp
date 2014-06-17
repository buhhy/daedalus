#include "Daedalus.h"
#include "Delaunay.h"
#include "Constants.h"
#include "DataStructures.h"

#include <deque>
#include <algorithm>

namespace utils {
	namespace delaunay {
		/**
		 * Returns 1: CW, 0: Colinear, -1: CCW
		 */
		int8 IsCWWinding(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			Vector2<> sub12 = v1->Point - v2->Point;
			Vector2<> sub32 = v3->Point - v2->Point;

			double result = sub12.Determinant(sub32);
			if (result < -FLOAT_ERROR) return -1;       // CCW
			if (result > FLOAT_ERROR) return 1;         // CW
			else return 0;                              // Colinear
		}

		uint64 Vertex::AddFace(Face * const face) {
			if (IncidentFace == NULL)
				IncidentFace = face;
			return ++FaceCount;
		}

		uint64 Vertex::RemoveFace(Face * const face) {
			if (face == IncidentFace) {
				if (FaceCount == 1)
					IncidentFace = NULL;
				else
					IncidentFace = face->GetAdjacentFaceCCW(this);
			}
			return --FaceCount;
		}

		/**
		 * Adjusts the adjacency pointer of the new face to point to the closest CCW face
		 * around a provided pivot point. This method returns a pair that indicates which
		 * existing face needs to have which adjacency index updated to point to the new
		 * face.
		 */
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
			pivotPoint->AddFace(newFace);
			if (otherFace == NULL)
				return std::make_pair((Face *) NULL, -1);

			// We need to find the face immediately clockwise and counter-clockwise of the
			// new face to adjust adjacencies. The face to the immediate CW of the new face
			// will need a pointer to the new face while the new face will link to the
			// immediate CCW face.
			
			double CCWMinAngle = 10.0, CWMinAngle = 10.0;
			int8 CWVertexIndex = -1;
			uint64 index = 0;
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
				index++;
			} while (index < pivotPoint->GetFaceCount());

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

		void DelaunayGraph::RemoveFace(Face * const face, const uint8 pivotIndex) {
			Vertex * const pivot = face->Vertices[pivotIndex];
			Face * const CCWFace = face->GetAdjacentFaceCCW(pivot);
			Face * const CWFace = face->GetAdjacentFaceCW(pivot);
			
			if (CWFace != face)
				CWFace->AdjacentFaces[CWFace->GetCCWVertexIndex(pivot)] = CCWFace;
			pivot->RemoveFace(face);
		}

		Vertex * DelaunayGraph::AddVertex(Vertex * const vertex) {
			Vertices.insert(vertex);
			return vertex;
		}

		Face * DelaunayGraph::AddFace(Vertex * const v1, Vertex * const v2) {
			Face * newFace = new Face(v1, v2);

			// Modify adjacencies
			std::array<std::pair<Face *, uint8>, 3> adjusts = {{
				AdjustNewFaceAdjacencies(newFace, 0),
				AdjustNewFaceAdjacencies(newFace, 1)
			}};

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
		Face * DelaunayGraph::AddFace(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			Vertex * inV1 = v1;
			Vertex * inV2 = v2;
			Vertex * inV3 = v3;

			// Insert face with clockwise vertex winding order
			auto winding = IsCWWinding(v1, v2, v3);
			if (winding < 0) {
				inV2 = v3;
				inV3 = v2;
			} else if (winding == 0) {
				// Don't create colinear triangles
				return NULL;
			}
			
			// Check if any degenerate faces exist with the current vertices, if so, adjust
			// that edge to become a triangle rather than creating one from scratch
			std::array<Face *, 3> faces = {{
				FindFace(v1, v2),
				FindFace(v2, v3),
				FindFace(v3, v1)
			}};

			for (auto f : faces)
				if (f != NULL && f->IsDegenerate) RemoveFace(f);

			Face * newFace = new Face(inV1, inV2, inV3);

			// Modify adjacencies
			std::array<std::pair<Face *, int8>, 3> adjusts = {{
				AdjustNewFaceAdjacencies(newFace, 0),
				AdjustNewFaceAdjacencies(newFace, 1),
				AdjustNewFaceAdjacencies(newFace, 2)
			}};

			for (auto adjust : adjusts) {
				if (adjust.first != NULL)
					adjust.first->AdjacentFaces[adjust.second] = newFace;
			}

			Faces.insert(newFace);

			return newFace;
		}

		bool DelaunayGraph::RemoveFace(Face * const face) {
			if (Faces.count(face) == 0)
				return false;
			for (uint8 i = 0; i < face->NumVertices; i++)
				RemoveFace(face, i);
			Faces.erase(face);
			delete face;
			return true;
		}
		
		Face * DelaunayGraph::FindFace(Vertex * const v1, Vertex * const v2) {
			Face * curFace = v1->IncidentFace;

			if (curFace == NULL)
				return NULL;

			bool found = false;
			do {
				found = curFace->FindVertex(v1) != -1 && curFace->FindVertex(v2) != -1;
				if (found)
					break;
				curFace = curFace->GetAdjacentFaceCCW(v1);
			} while (curFace != v1->IncidentFace);

			if (found)
				return curFace;
			else
				return NULL;
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

	uint64 ConvexHullGetSequenceCW(
		std::deque<Vertex *> & deque, const ConvexHull & hull,
		const uint64 start, const uint64 end
	) {
		uint64 count = (end - start + hull.size()) % hull.size() + 1;
		if (count == 1)
			count += hull.size();
		for (uint64 i = start, c = 0; c < count; c++, i++) {
			if (i >= hull.size()) i -= hull.size();
			deque.push_back(hull[i]);
		}
		return count;
	}

	uint64 ConvexHullGetSequenceCCW(
		std::deque<Vertex *> & deque, const ConvexHull & hull,
		const uint64 start, const uint64 end
	) {
		uint64 count = (start - end + hull.size()) % hull.size() + 1;
		uint64 c = 0;
		if (count == 1)
			count += hull.size();
		for (int64 i = start; c < count; c++, i--) {
			if (i < 0) i += hull.size();
			deque.push_back(hull[i]);
		}
		return count;
	}

	void MergeDelaunay(
		DelaunayGraph & results,
		const std::vector<Vertex *> & sortedVertices,
		const ConvexHull & leftHull,
		const ConvexHull & rightHull,
		const uint64 start1, const uint64 end1,
		const uint64 start2, const uint64 end2,
		const Tangent & upperTangent,
		const Tangent & lowerTangent
	) {
		bool left, right;
		bool takeLeft, takeRight;
		bool isLeftDone = false, isRightDone = false;

		std::vector<std::array<Vertex *, 3> > addedFaces;
		std::deque<Vertex *> leftVertexQueue;
		std::deque<Vertex *> rightVertexQueue;

		ConvexHullGetSequenceCCW(
			leftVertexQueue, leftHull, lowerTangent.first, upperTangent.first);
		ConvexHullGetSequenceCW(
			rightVertexQueue, rightHull, lowerTangent.second, upperTangent.second);

		Vertex * baseLeft = leftVertexQueue.front();
		Vertex * baseRight = rightVertexQueue.front();

		leftVertexQueue.pop_front();
		rightVertexQueue.pop_front();
		
		Vertex * leftCandidate = NULL, * rightCandidate = NULL, * nextCandidate = NULL;
		Face * faceIt = NULL;
		utils::Circle2D circumcircle;

		do {
			left = false, right = false;

			// Get left candidate
			leftCandidate = leftVertexQueue.front();
			faceIt = results.FindFace(leftCandidate, baseLeft);

			if (isLeftDone) {
				left = false;
			} else if (faceIt == NULL || faceIt->IsDegenerate) {
				left = IsStraight(baseRight->Point, baseLeft->Point, leftCandidate->Point) > 0;
			} else {
				do {
					// Only accept candidates that form angles < 180
					if (IsStraight(baseRight->Point, baseLeft->Point, leftCandidate->Point) <= 0)
						break;
					nextCandidate = faceIt->GetCCWVertex(leftCandidate);
					// If there is no next candidate, it must mean that there was no immediate face
					// counter-clockwise from the current face. Thus there is at least a 180 deg
					// angle to next possible candidate.
					if (nextCandidate == NULL)
						break;
					circumcircle = CalculateCircumcircle(
						leftCandidate->Point, baseRight->Point, baseLeft->Point);
					// For ambiguous points, don't delete
					if (IsWithinCircumcircle(nextCandidate->Point, circumcircle) > 0) {
						// If within circumcircle, we need to delete edge
						auto faceNext = faceIt->GetAdjacentFaceCCW(baseLeft);
						if (faceIt == faceNext) {
							// This is the only face attached to the left base vertex
							results.RemoveFace(faceIt);
							faceIt = NULL;
							left = true;
						} else {
							results.RemoveFace(faceIt);
							faceIt = faceNext;
						}
						leftVertexQueue.push_front(nextCandidate);
						leftCandidate = nextCandidate;
					} else {
						// Otherwise, this point is valid, and should be submitted as candidate
						left = true;
					}
				} while (faceIt != NULL && !left);
			}

			// Get right candidate
			rightCandidate = rightVertexQueue.front();
			faceIt = results.FindFace(rightCandidate, baseRight);

			if (isRightDone) {
				right = false;
			} else if (faceIt == NULL || faceIt->IsDegenerate) {
				right = IsStraight(rightCandidate->Point, baseRight->Point, baseLeft->Point) > 0;
			} else {
				do {
					// Only accept candidates that form angles < 180
					if (IsStraight(rightCandidate->Point, baseRight->Point, baseLeft->Point) <= 0)
						break;
					nextCandidate = faceIt->GetCWVertex(rightCandidate);
					// If there is no next candidate, it must mean that there was no immediate face
					// clockwise from the current face. Thus there is at least a 180 deg angle to
					// next possible candidate.
					if (nextCandidate == NULL)
						break;
					circumcircle = CalculateCircumcircle(
						rightCandidate->Point, baseRight->Point, baseLeft->Point);
					// For ambiguous points, don't delete
					if (IsWithinCircumcircle(nextCandidate->Point, circumcircle) > 0) {
						// If within circumcircle, we need to delete edge
						auto faceNext = faceIt->GetAdjacentFaceCW(baseRight);
						if (faceIt == faceNext) {
							// This is the only face attached to the right base vertex
							results.RemoveFace(faceIt);
							faceIt = NULL;
							right = true;
						} else {
							results.RemoveFace(faceIt);
							faceIt = faceNext;
						}
						rightVertexQueue.push_front(nextCandidate);
						rightCandidate = nextCandidate;
					} else {
						// Otherwise, this point is valid, and should be submitted as candidate
						right = true;
					}
				} while (faceIt != NULL && !right);
			}

			takeLeft = left; takeRight = right;

			if (takeLeft && takeRight) {
				// If 2 potential candidates, select one of the 2 candidates, left first
				circumcircle = CalculateCircumcircle(
					leftCandidate->Point, baseRight->Point, baseLeft->Point);

				if (left && IsWithinCircumcircle(rightCandidate->Point, circumcircle) > 0) {
					// If the right candidate is within the left circumcircle, right is selected
					takeLeft = false;
				} else {
				// Otherwise, left candidate is selected
					takeRight = false;
				}
			}

			if (takeRight) {
				addedFaces.push_back({{ rightCandidate, baseRight, baseLeft }});
				//rightHullIndex = BoundedAdd(rightHullIndex, rightHull.size());
				rightVertexQueue.pop_front();
				baseRight = rightCandidate;
			} else if (takeLeft) {
				addedFaces.push_back({{ leftCandidate, baseRight, baseLeft }});
				//leftHullIndex = BoundedSubtract(leftHullIndex, leftHull.size());
				leftVertexQueue.pop_front();
				baseLeft = leftCandidate;
			}

			isLeftDone = leftVertexQueue.empty();
			isRightDone = rightVertexQueue.empty();
		} while ((takeLeft || takeRight) && (!isLeftDone || !isRightDone));

		for (auto f : addedFaces)
			results.AddFace(f[0], f[1], f[2]);
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
		int8 winding;

		do {
			done = true;
			
			// Loop until the next left vertex is below the tangent
			while (true) {
				winding = IsCWWinding(leftHull[nextLeft], rightHull[rightIndex], leftHull[leftIndex]);
				// If the next left is below the tangent, or the 3 points are co-linear and the current left
				// is already higher than the next left, break
				if (winding < 0 ||
					(winding == 0 &&
						leftHull[leftIndex]->Point.Y >= leftHull[nextLeft]->Point.Y))
					break;
				nextLeft = BoundedSubtract(nextLeft, leftSize);
				leftIndex = BoundedSubtract(leftIndex, leftSize);
			}
			
			// Loop until the next right vertex is below the tangent
			while (true) {
				winding = IsCWWinding(rightHull[nextRight], rightHull[rightIndex], leftHull[leftIndex]);
				if (winding < 0 ||
					(winding == 0 &&
						rightHull[rightIndex]->Point.Y >= rightHull[nextRight]->Point.Y))
					break;
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
		int8 winding;

		do {
			done = true;

			// Loop until the next left vertex is above the tangent
			while (true) {
				winding = IsCWWinding(leftHull[leftIndex], rightHull[rightIndex], leftHull[nextLeft]);
				// If the next left is above the tangent, or the 3 points are co-linear and the current left
				// is already lower than the next left, break
				if (winding < 0 ||
					(winding == 0 &&
						leftHull[leftIndex]->Point.Y <= leftHull[nextLeft]->Point.Y))
					break;
				nextLeft = BoundedAdd(nextLeft, leftSize);
				leftIndex = BoundedAdd(leftIndex, leftSize);
			}
			
			// Loop until the next right vertex is above the tangent
			while (true) {
				winding = IsCWWinding(leftHull[leftIndex], rightHull[rightIndex], rightHull[nextRight]);
				if (winding < 0 ||
					(winding == 0 &&
						rightHull[rightIndex]->Point.Y <= rightHull[nextRight]->Point.Y))
					break;
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
					for (uint8 i = 0u; i < newFace->NumVertices; i++)
						hull.push_back(newFace->Vertices[i]);
				} else {
					// The 3 points are colinear, add 2 edges instead
					results.AddFace(
						sortedVertices[start],
						sortedVertices[start + 1]);
					results.AddFace(
						sortedVertices[start + 1],
						sortedVertices[start + 2]);
					for (uint8 i = 0u; i < 3; i++)
						hull.push_back(sortedVertices[start + i]);
				}
			} else if (count == 2) {
				// Otherwise, just create a line
				auto newFace = results.AddFace(
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
					results, sortedVertices,
					leftHull, rightHull,
					start, half, half + 1, end,
					upperTangent, lowerTangent);
			}
			return MergeConvexHulls(leftHull, rightHull, upperTangent, lowerTangent);
		}
	}

	void Test(DelaunayGraph & graph) {
		std::vector<Vector2<> > testPoints;
		std::vector<Vertex *> testVertices;
		testPoints.push_back({ 0.1, 0.2 });
		testPoints.push_back({ 0.2, 0.1 });
		testPoints.push_back({ 0.2, 0.3 });
		testPoints.push_back({ 0.2, 0.4 });
		testPoints.push_back({ 0.3, 0.2 });
		testPoints.push_back({ 0.4, 0.4 });
		testPoints.push_back({ 0.5, 0.3 });
		testPoints.push_back({ 0.6, 0.4 });
		testPoints.push_back({ 0.6, 0.2 });
		testPoints.push_back({ 0.6, 0.1 });

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

		graph.ConvexHull = Divide(graph, testVertices, 0, graph.VertexCount() - 1, 2);

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
			graph.ConvexHull = Divide(graph, copiedVertices, 0, graph.VertexCount() - 1, 0);
	}
}

#include <Daedalus.h>
#include "DelaunayDatastructures.h"

#include <algorithm>
#include <assert.h>

namespace utils {
	namespace delaunay {
		/*********************************************************************
		 * Vertex
		 *********************************************************************/


		Uint64 Vertex::AddFace(Face * const face) {
			IncidentFaces.insert({ face->FaceId(), face });
			// TODO: remove - this is simply for verification
			//IsSurrounded();

			return FaceCount();
		}

		Uint64 Vertex::RemoveFace(Face * const face) {
			assert(IncidentFaces.count(face->FaceId()) > 0 &&
				"Vertex::RemoveFace: vertex does not contain this face ID");

			IncidentFaces.erase(face->FaceId());
			
			// TODO: remove - this is simply for verification
			//IsSurrounded();

			return FaceCount();
		}

		bool Vertex::IsSurrounded() const {
			if (FaceCount() < 2)
				return false;

			Face * curFace = GetFirstIncidentFace();
			bool surrounded = true;

			for (Uint32 i = 0; i < FaceCount(); i++) {
				auto nextFace = curFace->GetAdjacentFaceCCW(this);
				auto curVertex = curFace->GetCWVertex(this);
				auto nextVertex = nextFace->GetCCWVertex(this);
				surrounded = surrounded && (*curVertex == *nextVertex);
				curFace = nextFace;

				if (nextFace == NULL)
					assert(!"Vertex::IsSurrounded: next face does not contain pivot index");
			}

			if (!(*GetFirstIncidentFace() == *curFace))
				assert(!"Vertex::IsSurrounded: face does not loop around vertex correctly");
			return surrounded;
		}


		/*********************************************************************
		 * Face
		 *********************************************************************/
		

		Int8 Face::FindVertex(Vertex const * const vertex) const {
			for (Int8 i = NumVertices - 1; i >= 0; i--)
				if (Vertices[i] == vertex) return i;
			return -1;
		}

		Int8 Face::FindFace(Face const * const face) const {
			for (Int8 i = NumVertices - 1; i >= 0; i--)
				if (AdjacentFaces[i] == face) return i;
			return -1;
		}

		Face * Face::GetAdjacentFaceCW(Vertex const * const sharedVertex) {
			// Since we don't have a pointer to the CW face, we need to loop around the
			// shared vertex until we find the last face before this face again
			Face * curFace = NULL;
			Face * nextFace = this;
			const auto fcount = sharedVertex->FaceCount();

			for (Uint64 c = 0; c < fcount; c++) {
				curFace = nextFace;
				nextFace = nextFace->GetAdjacentFaceCCW(sharedVertex);
				assert(nextFace != NULL &&
					"Face::GetAdjacentFaceCW: next face does not contain pivot index");
			}

			assert(curFace->GetAdjacentFaceCCW(sharedVertex) == this &&
				"Face::GetAdjacentFaceCW: face does not loop around vertex correctly");

			return curFace;
		}

		Face * Face::GetAdjacentFaceCCW(Vertex const * const sharedVertex) {
			Int8 found = FindVertex(sharedVertex);
			// Shared vertex doesn't actually exist in this face
			if (found == -1) return NULL;
			return AdjacentFaces[GetCCWVertexIndex(found)];
		}

		Circle2D Face::GetCircumcircle() const {
			if (IsDegenerate()) return Circle2D({ 0, 0 }, 0.0);
			return CalculateCircumcircle(
				Vertices[0]->GetPoint(), Vertices[1]->GetPoint(), Vertices[2]->GetPoint());
		}


		/*********************************************************************
		 * Convex Hull
		 *********************************************************************/
		
		bool ConvexHull::AddVertex(Vertex * const vert) {
			// Check for collinearity, already collinear hulls can continue being collinear
			// or cease being collinear, non-collinear hulls can never become collinear, hulls
			// with 2 or less vertices are considered not collinear, hence the collinear check
			// must occur when inserting the 3rd vertex
			if (Size() == 2 || bIsCollinear) {
				Vertex * const v0 = (*this)[0];
				Vertex * const v1 = (*this)[1];

				bIsCollinear = IsCWWinding(v0, vert, v1) == 0;
			}

			HullVertices.push_back(vert);

			return bIsCollinear;
		}
		
		Int32 ConvexHull::FindVertexById(const Uint64 id) const {
			for (size_t i = 0; i < Size(); i++)
				if (HullVertices[i]->VertexId() == id) return i;
			return -1;
		}

		Uint64 ConvexHull::GetSequence(
			std::deque<Vertex *> & deque,
			const Uint64 start, const Uint64 end,
			const bool isCW
		) const {
			Uint64 c = 0, size = Size();
			Uint64 count = GetRange(start, end, isCW);
			Int8 direction = (isCW ? 1 : -1);

			if (bIsCollinear) {
				// If the hull is collinear, iterate in the original intended direction, then
				// ping-pong when reaching an extreme rather than loop around
				for (Int64 i = start; c < count; c++, i += direction) {
					deque.push_back((*this)[(unsigned) i]);
					if ((direction < 0 && i == 0) || (direction > 0 && i == size - 1))
						direction *= -1;
				}
				return count;
			} else {
				for (Int64 i = start; c < count; c++, i += direction) {
					if (i >= (signed) size) i -= size;
					if (i < 0) i += size;
					deque.push_back((*this)[(unsigned) i]);
				}
				return count;
			}
		}

		Uint64 ConvexHull::GetRange(
			const Uint64 start, const Uint64 end,
			const bool isCW
		) const {
			Uint64 size = Size();
			Int8 direction = (isCW ? 1 : -1);
			if (bIsCollinear) {
				Int64 compare = direction * (end - start);
				Uint64 count;
				if (compare <= 0) {
					if (isCW)
						count = (2 * (size - 1)) - end - start + 1;
					else
						count = end + start + 1;
				} else {
					count = (unsigned)(compare + 1);
				}

				return count;
			} else {
				Uint64 count = (direction * (end - start) + size) % Size() + 1;
				if (count == 1)
					count += size;
				return count;
			}
		}

		utils::Vector2<> ConvexHull::Centroid() const {
			// Algorithm found here: http://en.wikipedia.org/wiki/Centroid#Centroid_of_polygon
			const auto size = HullVertices.size();
			
			utils::Vector2<> sum(0, 0);
			
			if (size < 3) {
				// If degenerate hull, then return average of the points
				for (auto & v : HullVertices)
					sum += v->GetPoint();
				return sum / (double) HullVertices.size();
			} else {
				// Total area formed by parallelograms of all triangles
				double totalArea = 0;

				for (Uint64 i = 0, j = 1; i < size; i++, j++) {
					if (j >= size) j -= size;
					const auto & p0 = HullVertices[i]->GetPoint();
					const auto & p1 = HullVertices[j]->GetPoint();
					// Dot product to find parallelogram area of triangle
					const double area = p0.X * p1.Y - p0.Y * p1.X;
					totalArea += area;
					sum.X += (p0.X + p1.X) * area;
					sum.Y += (p0.Y + p1.Y) * area;
				}

				totalArea /= 2.0;

				return sum / (6.0 * totalArea);
			}
		}

		Uint64 ConvexHull::FindTangent(const Vertex * compare, const bool isRight) const {
			if (Size() == 1)
				return 0;

			Uint64 index = 0;
			Uint64 prevIndex = isRight ? Size() - 1 : 1;
			Uint64 nextIndex = isRight ? 1 : Size() - 1;

			bool done = false;
			
			// Loop until no adjacent vertices are on the right side of the tangent
			do {
				done = false;

				const auto & vcur = (*this)[index];
				const auto & vnext = (*this)[nextIndex];
				const auto & vprev = (*this)[prevIndex];
			
				Int8 prevWinding = isRight ?
					IsCWWinding(compare, vcur, vprev) : IsCWWinding(compare, vprev, vcur);
				Int8 nextWinding = isRight ?
					IsCWWinding(compare, vcur, vnext) : IsCWWinding(compare, vnext, vcur);

				// If the both next and prev vertices are left of the current point, then
				// we've reached the apex.
				if (nextWinding < 0 && prevWinding < 0) {
					done = true;
				} else if (prevWinding == 0 || nextWinding == 0) {
					// In the case of collinear left, next left, and right points, select
					// the point with the shortest distance.
					double curDist = (vcur->GetPoint() - compare->GetPoint()).Length2();
					double prevDist = curDist + 1;
					double nextDist = curDist + 1;

					if (prevWinding == 0)
						prevDist = (vprev->GetPoint() - compare->GetPoint()).Length2();
					if (nextWinding == 0)
						nextDist = (vnext->GetPoint() - compare->GetPoint()).Length2();

					if (prevDist < nextDist && prevDist < curDist)
						index = prevIndex;
					else if (nextDist < prevDist && nextDist < curDist)
						index = nextIndex;
					else
						done = true;
				} else {
					if (nextWinding == 1)
						index = nextIndex;
					else
						index = prevIndex;
				}

				prevIndex = isRight ? this->PrevIndex(index) : this->NextIndex(index);
				nextIndex = isRight ? this->NextIndex(index) : this->PrevIndex(index);
			} while (!done);

			return index;
		}
	}

	using namespace delaunay;


	/*********************************************************************
	 * Delaunay Graph
	 *********************************************************************/


	DelaunayGraph::DelaunayGraph(const DelaunayGraph & copy) :
		CurrentFaceId(copy.CurrentFaceId),
		CurrentVertexId(copy.CurrentVertexId),
		Offset(copy.Offset)
	{
		// Add all vertices and faces in the copy graph
		for (auto it : copy.Vertices)
			AddVertexToCache(new delaunay::Vertex(*it));
		// TODO: this doesn't deep copy right now
		for (auto it : copy.Faces)
			AddFaceToCache(new delaunay::Face(*it));

		// Set adjacencies for all faces and vertices
		// TODO: this is not working properly yet
		//for (auto it : copy.Vertices) {
		//	IdVertexMap.at(it->VertexId())->GetIncidentFace() =
		//		IdFaceMap.at(it->GetIncidentFace()->FaceId());
		//}

		for (auto it : copy.Faces) {
			auto & face = IdFaceMap.at(it->FaceId());
			for (Uint8 i = 0; i < face->VertexCount(); i++)
				face->AdjacentFaces[i] = IdFaceMap.at(it->AdjacentFaces[i]->FaceId());
		}

		// Set the convex hull
		std::vector<delaunay::Vertex *> newHullVerts;
		std::transform(
			ConvexHull.CBegin(),
			ConvexHull.CEnd(),
			newHullVerts.begin(),
			[&] (delaunay::Vertex * const vert) {
				return IdVertexMap.at(vert->VertexId());
			});
		ConvexHull = newHullVerts;
	}
	
	std::pair<Face *, Int8> DelaunayGraph::AdjustNewFaceAdjacencies(
		Face * const newFace,
		const Uint8 pivotIndex
	) {
		Int8 pivotCWIndex = newFace->GetCWVertexIndex(pivotIndex);
		Int8 pivotCCWIndex = newFace->GetCCWVertexIndex(pivotIndex);

		const auto pivotPoint = newFace->Vertices[pivotIndex];
		const auto pivotCWPoint = newFace->Vertices[pivotCWIndex];
		const auto pivotCCWPoint = newFace->Vertices[pivotCCWIndex];

		// If pivot point has no faces yet, assign the new one
		if (pivotPoint->FaceCount() == 0)
			return std::make_pair((Face *) NULL, -1);

		Face * otherFace = pivotPoint->GetFirstIncidentFace();

		// We need to find the face immediately clockwise and counter-clockwise of the
		// new face to adjust adjacencies. The face to the immediate CW of the new face
		// will need a pointer to the new face while the new face will link to the
		// immediate CCW face.
			
		double CCWMinAngle = 10.0, CWMinAngle = 10.0;
		Int8 CWVertexIndex = -1;
		Uint64 index = 0;
		Face * CCWFace = NULL, * CWFace = NULL;

		Vector2<> CCWCompareEdge = pivotCWPoint->GetPoint() - pivotPoint->GetPoint();
		Vector2<> CWCompareEdge = pivotCCWPoint->GetPoint() - pivotPoint->GetPoint();

		// Traverse faces currently at pivot point in CCW fashion, keeping the smallest
		// available CCW angle, as well as the smallest available CW angle.
		do {
			Int8 otherPivotIndex = otherFace->FindVertex(pivotPoint);
			Int8 otherPivotCWIndex = otherFace->GetCWVertexIndex(otherPivotIndex);
			Int8 otherPivotCCWIndex = otherFace->GetCCWVertexIndex(otherPivotIndex);

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
				auto angle = FindAngle(
					CCWCompareEdge, otherPivotCCW->GetPoint() - pivotPoint->GetPoint());
				// Account for rounding errors?
				if (angle > MATH_PI * 2 - FLOAT_ERROR) angle = 0;
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
				auto angle = FindAngle(otherPivotCW->GetPoint() - pivotPoint->GetPoint(), CWCompareEdge);
				// Account for rounding errors?
				if (angle > MATH_PI * 2 - FLOAT_ERROR) angle = 0;
				if (angle < CWMinAngle) {
					CWMinAngle = angle;
					CWFace = otherFace;
					CWVertexIndex = otherPivotCCWIndex;
				}
			}

			otherFace = otherFace->GetAdjacentFaceCCW(pivotPoint);
			index++;
		} while (index < pivotPoint->FaceCount());

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

	void DelaunayGraph::AdjustRemovedFaceAdjacencies(Face * const face, const Uint8 pivotIndex) {
		Vertex * const pivot = face->Vertices[pivotIndex];
		Face * const CCWFace = face->GetAdjacentFaceCCW(pivot);
		Face * const CWFace = face->GetAdjacentFaceCW(pivot);

		// The CCW face and CW face should never be equal unless that is the only other
		// face attached to the pivot vertex.
		if (pivot->FaceCount() == 2 && *CCWFace != *CWFace)
			assert(!"DelaunayGraph::AdjustRemovedFaceAdjacencies: CW and CCW faces should be same if there are exactly 2 faces on the pivot");
		if (pivot->FaceCount() > 2 && *CCWFace == *CWFace)
			assert(!"DelaunayGraph::AdjustRemovedFaceAdjacencies: CW and CCW faces should not be the same if there are more than 2 faces on the pivot");

		// The CCW face and CW face should never be equal to the removed face, unless the
		// removed face is the only face attached to this pivot vertex.
		if (pivot->FaceCount() == 1 && (*CCWFace != *face || *CWFace != *face))
			assert(!"DelaunayGraph::AdjustRemovedFaceAdjacencies: CW and CCW and the removed face should be the same if there is only 1 face on the pivot");
		if (pivot->FaceCount() > 1 && (*CCWFace == *face || *CWFace == *face))
			assert(!"DelaunayGraph::AdjustRemovedFaceAdjacencies: CW and CCW and the removed face should not be the same if there are other faces");
			
		CWFace->AdjacentFaces[CWFace->GetCCWVertexIndex(pivot)] = CCWFace;
	}

	Uint64 DelaunayGraph::GetNextFaceId() { return CurrentFaceId++; }
	Uint64 DelaunayGraph::GetNextVertexId() { return CurrentVertexId++; }

	Vertex * DelaunayGraph::AddVertexToCache(Vertex * const vertex) {
		auto id = vertex->VertexId();
		Vertices.insert(vertex);
		IdVertexMap.insert({ id, vertex });
		if (vertex->IsForeign())
			ForeignIdVertexMap.insert({
				GhostId(vertex->ParentGraphOffset(), vertex->ForeignVertexId()), vertex});
		if (id >= CurrentVertexId)
			CurrentVertexId = id + 1;
		return vertex;
	}

	Face * DelaunayGraph::AddFaceToCache(Face * const face) {
		auto id = face->FaceId();
		Faces.insert(face);
		IdFaceMap.insert({ id, face });
		if (id >= CurrentFaceId)
			CurrentFaceId = id + 1;
		return face;
	}
	
	bool DelaunayGraph::RemoveFaceFromCache(delaunay::Face * const face) {
		if (IdFaceMap.count(face->FaceId()) == 0)
			return false;
		Faces.erase(face);
		IdFaceMap.erase(face->FaceId());
		return true;
	}

	delaunay::Vertex * DelaunayGraph::AddGhostVertex(delaunay::Vertex * const vertex) {
		if (vertex->ParentGraphOffset() == Offset) {
			// Local vertex
			return vertex;
		} else {
			// Foreign vertex, we need to potentially duplicate if this vertex doesn't already
			// exist in the cache, and change ownership, and add offset
			auto existingVertex = ForeignIdVertexMap.find({
				vertex->ParentGraphOffset(), vertex->VertexId() });

			if (existingVertex == ForeignIdVertexMap.end()) {
				auto point = vertex->GetPoint();
				auto offset = vertex->ParentGraphOffset() - Offset;
				auto lid = GetNextVertexId();
				auto newVertex = new Vertex(
					vertex->ParentGraphOffset(),
					{ point.X + offset.X, point.Y + offset.Y },
					lid, vertex->VertexId());

				AddVertexToCache(newVertex);
				return newVertex;
			} else {
				return existingVertex->second;
			}
		}
	}

	Vertex * DelaunayGraph::AddVertex(const Vector2<> & point, const Uint64 id) {
		return AddVertexToCache(new Vertex(Offset, point, id));
	}

	Face * DelaunayGraph::AddFace(Vertex * const v1, Vertex * const v2) {
		Face * newFace = new Face(v1, v2, GetNextFaceId());

		// Modify adjacencies
		std::array<std::pair<Face *, Uint8>, 3> adjusts = {{
			AdjustNewFaceAdjacencies(newFace, 0),
			AdjustNewFaceAdjacencies(newFace, 1)
		}};

		for (auto adjust : adjusts) {
			if (adjust.first != NULL)
				adjust.first->AdjacentFaces[adjust.second] = newFace;
		}

		// Add new face to face vertices
		for (Uint8 i = 0; i < newFace->VertexCount(); i++)
			newFace->Vertices[i]->AddFace(newFace);

		AddFaceToCache(newFace);

		return newFace;
	}

	Face * DelaunayGraph::AddFace(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
		// Ensure we aren't adding a triangle to vertices that are already surrounded. This
		// would indicate an overlapping triangle.
		if (v1->IsSurrounded() || v2->IsSurrounded() || v3->IsSurrounded())
			assert(!"DelaunayGraph::AddFace: attempting to add a triangle to a surrounded vertex");

		Vertex * inV1 = AddGhostVertex(v1);
		Vertex * inV2 = AddGhostVertex(v2);
		Vertex * inV3 = AddGhostVertex(v3);

		// Insert face with clockwise vertex winding order
		auto winding = IsCWWinding(inV1, inV2, inV3);
		if (winding < 0) {
			std::swap(inV2, inV3);
		} else if (winding == 0) {
			// Don't create colinear triangles
			return NULL;
		}
			
		// Check if any degenerate faces exist with the current vertices, if so, adjust
		// that edge to become a triangle rather than creating one from scratch
		std::array<Face *, 3> faces = {{
			FindFace(inV1, inV2),
			FindFace(inV2, inV3),
			FindFace(inV3, inV1)
		}};

		for (auto f : faces)
			if (f != NULL && f->IsDegenerate()) RemoveFace(f);

		Face * newFace = new Face(inV1, inV2, inV3, GetNextFaceId());

		// Modify adjacencies
		std::array<std::pair<Face *, Int8>, 3> adjusts = {{
			AdjustNewFaceAdjacencies(newFace, 0),
			AdjustNewFaceAdjacencies(newFace, 1),
			AdjustNewFaceAdjacencies(newFace, 2)
		}};

		for (auto adjust : adjusts) {
			if (adjust.first != NULL)
				adjust.first->AdjacentFaces[adjust.second] = newFace;
		}

		// Add new face to face vertices
		for (Uint8 i = 0; i < newFace->VertexCount(); i++)
			newFace->Vertices[i]->AddFace(newFace);

		AddFaceToCache(newFace);

		return newFace;
	}

	bool DelaunayGraph::RemoveFace(Face * const face) {
		if (Faces.count(face) == 0)
			return false;
		for (Uint8 i = 0; i < face->VertexCount(); i++)
			AdjustRemovedFaceAdjacencies(face, i);
		for (Uint8 i = 0; i < face->VertexCount(); i++)
			face->Vertices[i]->RemoveFace(face);
		RemoveFaceFromCache(face);
		delete face;
		return true;
	}
		
	Face * DelaunayGraph::FindFace(Vertex const * const v1, Vertex const * const v2) {
		Face * curFace = v1->GetFirstIncidentFace();

		if (v1->FaceCount() == 0 || v2->FaceCount() == 0)
			return NULL;

		bool found = false;
		Face * foundFace = NULL;
		for (Uint64 c = 0; c < v1->FaceCount(); c++) {
			found = curFace->FindVertex(v1) != -1 && curFace->FindVertex(v2) != -1;
			if (found && (foundFace == NULL || foundFace->FaceId() < curFace->FaceId()))
				foundFace = curFace;
			curFace = curFace->GetAdjacentFaceCCW(v1);
		}

		if (*curFace != *v1->GetFirstIncidentFace())
			assert(!"DelaunayGraph::FindFace: face does not loop around vertex correctly");

		return foundFace;
	}
	
	const delaunay::Vertex * DelaunayGraph::FindVertex(const Uint64 vid) const {
		if (IdVertexMap.find(vid) == IdVertexMap.end())
			return NULL;
		return IdVertexMap.at(vid);
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

	const std::unordered_set<Edge> DelaunayGraph::GetUniqueEdges() const {
		std::unordered_set<Edge> edgeSet;

		for (auto it : Faces) {
			edgeSet.insert({ it->Vertices[0], it->Vertices[1] });
			if (!it->IsDegenerate()) {
				edgeSet.insert({ it->Vertices[0], it->Vertices[2] });
				edgeSet.insert({ it->Vertices[1], it->Vertices[2] });
			}
		}

		return edgeSet;
	}
}

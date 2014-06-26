#include <Daedalus.h>
#include "DelaunayDatastructures.h"

#include <algorithm>
#include <assert.h>

namespace utils {
	namespace delaunay {
		/*********************************************************************
		 * Vertex
		 *********************************************************************/


		uint64_t Vertex::AddFace(Face * const face) {
			if (IncidentFace == NULL)
				IncidentFace = face;
			// TODO: remove - this is simply for verification
			++NumFaces;
			IsSurrounded();

			return NumFaces;
		}

		uint64_t Vertex::RemoveFace(Face * const face) {
			if (face == IncidentFace) {
				if (NumFaces == 1)
					IncidentFace = NULL;
				else
					IncidentFace = face->GetAdjacentFaceCCW(this);
			}

			--NumFaces;

			if (IncidentFace != NULL)
				IsSurrounded();

			return NumFaces;
		}

		bool Vertex::IsSurrounded() const {
			Face * curFace = GetIncidentFace();
			bool surrounded = true;

			for (uint32_t i = 0; i < FaceCount(); i++) {
				auto nextFace = curFace->GetAdjacentFaceCCW(this);
				auto curVertex = curFace->GetCWVertex(this);
				auto nextVertex = nextFace->GetCCWVertex(this);
				surrounded = surrounded && (*curVertex == *nextVertex);
				curFace = nextFace;

				if (nextFace == NULL)
					assert(!"Vertex::IsSurrounded: next face does not contain pivot index");
			}

			if (!(*GetIncidentFace() == *curFace))
				assert(!"Vertex::IsSurrounded: face does not loop around vertex correctly");
			return surrounded;
		}


		/*********************************************************************
		 * Face
		 *********************************************************************/
		

		int8_t Face::FindVertex(Vertex const * const vertex) const {
			for (int8_t i = NumVertices - 1; i >= 0; i--)
				if (Vertices[i] == vertex) return i;
			return -1;
		}

		int8_t Face::FindFace(Face const * const face) const {
			for (int8_t i = NumVertices - 1; i >= 0; i--)
				if (AdjacentFaces[i] == face) return i;
			return -1;
		}

		Face * Face::GetAdjacentFaceCW(Vertex const * const sharedVertex) {
			// Since we don't have a pointer to the CW face, we need to loop around the
			// shared vertex until we find the last face before this face again
			Face * curFace = NULL;
			Face * nextFace = this;
			const auto fcount = sharedVertex->FaceCount();

			for (uint64_t c = 0; c < fcount; c++) {
				curFace = nextFace;
				nextFace = nextFace->GetAdjacentFaceCCW(sharedVertex);
				assert(nextFace != NULL &&
					"Face::GetAdjacentFaceCW: next face does not contain pivot index");
			}

			assert(curFace->GetAdjacentFaceCCW(sharedVertex) == this &&
				"Face::GetAdjacentFaceCW: face does not loop around vertex correctly");

			return nextFace == NULL ? NULL : curFace;
		}

		Face * Face::GetAdjacentFaceCCW(Vertex const * const sharedVertex) {
			int8_t found = FindVertex(sharedVertex);
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
		
		int32_t ConvexHull::FindVertexById(const uint32_t id) const {
			for (uint32_t i = 0; i < Size(); i++)
				if (HullVertices[i]->VertexId() == id) return i;
			return -1;
		}
		
		uint32_t ConvexHull::LeftVertexIndex() const {
			return MinIndex([] (Vertex * const v) { return v->GetPoint().X; });
		}
		
		uint32_t ConvexHull::RightVertexIndex() const {
			return MinIndex([] (Vertex * const v) { return -v->GetPoint().X; });
		}
		
		uint32_t ConvexHull::TopVertexIndex() const {
			return MinIndex([] (Vertex * const v) { return v->GetPoint().Y; });
		}
		
		uint32_t ConvexHull::BottomVertexIndex() const {
			return MinIndex([] (Vertex * const v) { return -v->GetPoint().Y; });
		}

		uint32_t ConvexHull::MinIndex(const std::function<double (Vertex * const)> & valueOf) const {
			if (HullVertices.empty())
				return 0;
			uint32_t minIndex = 0;
			double threshold = valueOf(HullVertices[0]);
			for (uint32_t i = 1u; i < HullVertices.size(); i++) {
				auto value = valueOf(HullVertices[i]);
				if (value < threshold) {
					minIndex = i;
					threshold = value;
				}
			}
			return minIndex;
		}

		uint32_t ConvexHull::GetSequence(
			std::deque<Vertex *> & deque,
			const uint32_t start, const uint32_t end,
			const bool isCW
		) const {
			uint32_t c = 0, size = Size();
			uint32_t count = GetRange(start, end, isCW);
			int8_t direction = (isCW ? 1 : -1);

			if (bIsCollinear) {
				// If the hull is collinear, iterate in the original intended direction, then
				// ping-pong when reaching an extreme rather than loop around
				for (int64_t i = start; c < count; c++, i += direction) {
					deque.push_back((*this)[(unsigned) i]);
					if (direction < 0 && i == 0 || direction > 0 && i == size - 1)
						direction *= -1;
				}
				return count;
			} else {
				for (int64_t i = start; c < count; c++, i += direction) {
					if (i >= (signed) size) i -= size;
					if (i < 0) i += size;
					deque.push_back((*this)[(unsigned) i]);
				}
				return count;
			}
		}

		uint32_t ConvexHull::GetRange(
			const uint32_t start, const uint32_t end,
			const bool isCW
		) const {
			uint32_t size = Size();
			int8_t direction = (isCW ? 1 : -1);
			if (bIsCollinear) {
				int64_t compare = direction * (end - start);
				uint32_t count;
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
				uint32_t count = (direction * (end - start) + size) % Size() + 1;
				if (count == 1)
					count += size;
				return count;
			}
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
			AddVertex(new delaunay::Vertex(*it));
		// TODO: this doesn't deep copy right now
		for (auto it : copy.Faces)
			AddFace(new delaunay::Face(*it));

		// Set adjacencies for all faces and vertices
		// TODO: this is not working properly yet
		//for (auto it : copy.Vertices) {
		//	IdVertexMap.at(it->VertexId())->GetIncidentFace() =
		//		IdFaceMap.at(it->GetIncidentFace()->FaceId());
		//}

		for (auto it : copy.Faces) {
			auto & face = IdFaceMap.at(it->FaceId());
			for (uint8_t i = 0; i < face->VertexCount(); i++)
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
	
	std::pair<Face *, int8_t> DelaunayGraph::AdjustNewFaceAdjacencies(
		Face * const newFace,
		const uint8_t pivotIndex
	) {
		int8_t pivotCWIndex = newFace->GetCWVertexIndex(pivotIndex);
		int8_t pivotCCWIndex = newFace->GetCCWVertexIndex(pivotIndex);

		const auto pivotPoint = newFace->Vertices[pivotIndex];
		const auto pivotCWPoint = newFace->Vertices[pivotCWIndex];
		const auto pivotCCWPoint = newFace->Vertices[pivotCCWIndex];

		Face * otherFace = pivotPoint->GetIncidentFace();

		// If pivot point has no faces yet, assign the new one
		if (otherFace == NULL)
			return std::make_pair((Face *) NULL, -1);

		// We need to find the face immediately clockwise and counter-clockwise of the
		// new face to adjust adjacencies. The face to the immediate CW of the new face
		// will need a pointer to the new face while the new face will link to the
		// immediate CCW face.
			
		double CCWMinAngle = 10.0, CWMinAngle = 10.0;
		int8_t CWVertexIndex = -1;
		uint64_t index = 0;
		Face * CCWFace = NULL, * CWFace = NULL;

		Vector2<> CCWCompareEdge = pivotCWPoint->GetPoint() - pivotPoint->GetPoint();
		Vector2<> CWCompareEdge = pivotCCWPoint->GetPoint() - pivotPoint->GetPoint();

		// Traverse faces currently at pivot point in CCW fashion, keeping the smallest
		// available CCW angle, as well as the smallest available CW angle.
		do {
			int8_t otherPivotIndex = otherFace->FindVertex(pivotPoint);
			int8_t otherPivotCWIndex = otherFace->GetCWVertexIndex(otherPivotIndex);
			int8_t otherPivotCCWIndex = otherFace->GetCCWVertexIndex(otherPivotIndex);

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
				if (angle > M_PI * 2 - 1E-3) angle = 0;
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
				if (angle > M_PI * 2 - FLOAT_ERROR) angle = 0;
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

	void DelaunayGraph::AdjustRemovedFaceAdjacencies(Face * const face, const uint8_t pivotIndex) {
		Vertex * const pivot = face->Vertices[pivotIndex];
		Face * const CCWFace = face->GetAdjacentFaceCCW(pivot);
		Face * const CWFace = face->GetAdjacentFaceCW(pivot);
			
		if (CWFace != face)
			CWFace->AdjacentFaces[CWFace->GetCCWVertexIndex(pivot)] = CCWFace;
		pivot->RemoveFace(face);
	}

	uint64_t DelaunayGraph::GetNextFaceId() { return CurrentFaceId++; }
	uint64_t DelaunayGraph::GetNextVertexId() { return CurrentVertexId++; }

	Vertex * DelaunayGraph::AddVertex(Vertex * const vertex) {
		auto id = vertex->VertexId();
		Vertices.insert(vertex);
		IdVertexMap.insert({ id, vertex });
		if (id>= CurrentVertexId)
			CurrentVertexId = id + 1;
		return vertex;
	}

	Face * DelaunayGraph::AddFace(Face * const face) {
		auto id = face->FaceId();
		Faces.insert(face);
		IdFaceMap.insert({ id, face });
		if (id >= CurrentFaceId)
			CurrentFaceId = id + 1;
		return face;
	}

	delaunay::Vertex * DelaunayGraph::AddGhostVertex(delaunay::Vertex * const vertex) {
		if (vertex->ParentGraphOffset() == Offset) {
			// Local vertex
			return vertex;
		} else {
			// Foreign vertex, we need to duplicate and change ownernship, and add offset
			auto point = vertex->GetPoint();
			auto offset = vertex->ParentGraphOffset() - Offset;
			auto lid = GetNextVertexId();
			auto newVertex = new Vertex(
				vertex->ParentGraphOffset(),
				{ point.X + offset.X, point.Y + offset.Y },
				lid, vertex->VertexId());
			Vertices.insert(newVertex);
			IdVertexMap.insert({ lid, newVertex });
			return newVertex;
		}
	}

	Vertex * DelaunayGraph::AddVertex(const Vector2<> & point, const uint64_t id) {
		return AddVertex(new Vertex(Offset, point, id));
	}

	Face * DelaunayGraph::AddFace(Vertex * const v1, Vertex * const v2) {
		Face * newFace = new Face(v1, v2, GetNextFaceId());

		// Modify adjacencies
		std::array<std::pair<Face *, uint8_t>, 3> adjusts = {{
			AdjustNewFaceAdjacencies(newFace, 0),
			AdjustNewFaceAdjacencies(newFace, 1)
		}};

		for (auto adjust : adjusts) {
			if (adjust.first != NULL)
				adjust.first->AdjacentFaces[adjust.second] = newFace;
		}

		// Add new face to face vertices
		for (uint8_t i = 0; i < newFace->VertexCount(); i++)
			newFace->Vertices[i]->AddFace(newFace);

		AddFace(newFace);

		return newFace;
	}

	Face * DelaunayGraph::AddFace(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
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
		
		// TODO: remove breakpoint hook
		if (newFace->FaceId() == 690)
			int i = 5;

		// Modify adjacencies
		std::array<std::pair<Face *, int8_t>, 3> adjusts = {{
			AdjustNewFaceAdjacencies(newFace, 0),
			AdjustNewFaceAdjacencies(newFace, 1),
			AdjustNewFaceAdjacencies(newFace, 2)
		}};

		for (auto adjust : adjusts) {
			if (adjust.first != NULL)
				adjust.first->AdjacentFaces[adjust.second] = newFace;
		}

		// Add new face to face vertices
		for (uint8_t i = 0; i < newFace->VertexCount(); i++)
			newFace->Vertices[i]->AddFace(newFace);

		AddFace(newFace);

		return newFace;
	}

	bool DelaunayGraph::RemoveFace(Face * const face) {
		if (Faces.count(face) == 0)
			return false;
		for (uint8_t i = 0; i < face->VertexCount(); i++)
			AdjustRemovedFaceAdjacencies(face, i);
		Faces.erase(face);
		IdFaceMap.erase(face->FaceId());
		delete face;
		return true;
	}
		
	Face * DelaunayGraph::FindFace(Vertex const * const v1, Vertex const * const v2) {
		Face * curFace = v1->GetIncidentFace();

		if (curFace == NULL)
			return NULL;

		bool found = false;
		Face * foundFace = NULL;
		for (uint64_t c = 0; c < v1->FaceCount(); c++) {
			found = curFace->FindVertex(v1) != -1 && curFace->FindVertex(v2) != -1;
			if (found && (foundFace == NULL || foundFace->FaceId() < curFace->FaceId()))
				foundFace = curFace;
			curFace = curFace->GetAdjacentFaceCCW(v1);
		}

		if (!(*curFace == *v1->GetIncidentFace()))
			assert(!"DelaunayGraph::FindFace: face does not loop around vertex correctly");

		return foundFace;
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
			if (!it->IsDegenerate()) {
				edgeSet.insert({ it->Vertices[0], it->Vertices[2] });
				edgeSet.insert({ it->Vertices[1], it->Vertices[2] });
			}
		}

		for (auto it : edgeSet) ret.push_back(it);

		return ret;
	}
}

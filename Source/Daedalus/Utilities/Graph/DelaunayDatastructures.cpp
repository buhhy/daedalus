#include "Daedalus.h"
#include "DelaunayDatastructures.h"

#include <algorithm>

namespace utils {
	namespace delaunay {
		/*********************************************************************
		 * Vertex
		 *********************************************************************/


		uint64 Vertex::AddFace(Face * const face) {
			if (IncidentFace == NULL)
				IncidentFace = face;
			return ++NumFaces;
		}

		uint64 Vertex::RemoveFace(Face * const face) {
			if (face == IncidentFace) {
				if (NumFaces == 1)
					IncidentFace = NULL;
				else
					IncidentFace = face->GetAdjacentFaceCCW(this);
			}
			return --NumFaces;
		}


		/*********************************************************************
		 * Face
		 *********************************************************************/
		

		int8 Face::FindVertex(Vertex * const vertex) const {
			for (int8 i = NumVertices - 1; i >= 0; i--)
				if (Vertices[i] == vertex) return i;
			return -1;
		}

		int8 Face::FindFace(Face * const face) const {
			for (int8 i = NumVertices - 1; i >= 0; i--)
				if (AdjacentFaces[i] == face) return i;
			return -1;
		}

		Face * Face::GetAdjacentFaceCW(Vertex * const sharedVertex) {
			// Since we don't have a pointer to the CW face, we need to loop around the
			// shared vertex until we find the last face before this face again
			Face * curFace;
			Face * nextFace = this;

			do {
				curFace = nextFace;
				nextFace = nextFace->GetAdjacentFaceCCW(sharedVertex);
			} while (nextFace != this && nextFace != NULL);

			return nextFace == NULL ? NULL : curFace;
		}

		Face * Face::GetAdjacentFaceCCW(Vertex * const sharedVertex) {
			int8 found = FindVertex(sharedVertex);
			// Shared vertex doesn't actually exist in this face
			if (found == -1) return NULL;
			return AdjacentFaces[GetCCWVertexIndex(found)];
		}

		Circle2D Face::GetCircumcircle() const {
			if (IsDegenerate()) return Circle2D({ 0, 0 }, 0.0);
			return CalculateCircumcircle(
				Vertices[0]->Point, Vertices[1]->Point, Vertices[2]->Point);
		}


		/*********************************************************************
		 * Convex Hull
		 *********************************************************************/

		
		int64 ConvexHull::LeftVertexIndex() const {
			return MinIndex([] (Vertex * const v) { return v->Point.X; });
		}
		
		int64 ConvexHull::RightVertexIndex() const {
			return MinIndex([] (Vertex * const v) { return -v->Point.X; });
		}
		
		int64 ConvexHull::TopVertexIndex() const {
			return MinIndex([] (Vertex * const v) { return v->Point.Y; });
		}
		
		int64 ConvexHull::BottomVertexIndex() const {
			return MinIndex([] (Vertex * const v) { return -v->Point.Y; });
		}

		int64 ConvexHull::MinIndex(std::function<double (Vertex * const)> valueOf) const {
			if (HullVertices.empty())
				return -1;
			uint64 minIndex = 0;
			double threshold = valueOf(HullVertices[0]);
			for (uint64 i = 1u; i < HullVertices.size(); i++) {
				auto value = valueOf(HullVertices[i]);
				if (value < threshold) {
					minIndex = i;
					threshold = value;
				}
			}
			return minIndex;
		}
	}

	using namespace delaunay;


	/*********************************************************************
	 * Delaunay Graph
	 *********************************************************************/


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
				auto angle = FindAngle(otherPivotCW->Point - pivotPoint->Point, CWCompareEdge);
				// Account for rounding errors?
				if (angle > M_PI * 2 - 1E-3) angle = 0;
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

	void DelaunayGraph::RemoveFace(Face * const face, const uint8 pivotIndex) {
		Vertex * const pivot = face->Vertices[pivotIndex];
		Face * const CCWFace = face->GetAdjacentFaceCCW(pivot);
		Face * const CWFace = face->GetAdjacentFaceCW(pivot);
			
		if (CWFace != face)
			CWFace->AdjacentFaces[CWFace->GetCCWVertexIndex(pivot)] = CCWFace;
		pivot->RemoveFace(face);
	}

	uint64 DelaunayGraph::GetNextFaceId() {
		return CurrentFaceId++;
	}

	Vertex * DelaunayGraph::AddVertex(Vertex * const vertex) {
		Vertices.insert(vertex);
		return vertex;
	}

	Face * DelaunayGraph::AddFace(Vertex * const v1, Vertex * const v2) {
		Face * newFace = new Face(v1, v2, GetNextFaceId());

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
			if (f != NULL && f->IsDegenerate()) RemoveFace(f);

		Face * newFace = new Face(inV1, inV2, inV3, GetNextFaceId());

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
		for (uint8 i = 0; i < face->VertexCount(); i++)
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
			if (!it->IsDegenerate()) {
				edgeSet.insert({ it->Vertices[0], it->Vertices[2] });
				edgeSet.insert({ it->Vertices[1], it->Vertices[2] });
			}
		}

		for (auto it : edgeSet) ret.push_back(it);

		return ret;
	}
}

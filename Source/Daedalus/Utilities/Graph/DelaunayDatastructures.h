#pragma once

#include "Engine.h"
#include "DataStructures.h"

#include <array>
#include <unordered_set>
#include <functional>

namespace utils {
	namespace delaunay {
		struct Face;
		class ConvexHull;

		class Vertex {
		private:
			uint64 FaceCount;

		public:
			Vector2<> Point;
			Face * IncidentFace;
			uint64 VertexId;

			Vertex(const Vector2<> & point, uint64 id) :
				Point(point), IncidentFace(NULL), VertexId(id), FaceCount(0)
			{}

			uint64 AddFace(Face * const face);
			uint64 RemoveFace(Face * const face);

			inline uint64 GetFaceCount() const { return FaceCount; }

			inline bool operator == (const Vertex & other) const {
				return other.VertexId == VertexId;
			}
		};

		struct Edge {
			Vertex * Start;
			Vertex * End;

			Edge(Vertex * const start, Vertex * const end) {
				if (start->VertexId < end->VertexId) {
					Start = start;
					End = end;
				} else {
					Start = end;
					End = start;
				}	
			}

			inline bool operator == (const Edge & other) const {
				return *other.Start == *Start && *other.End == *End;
			}
		};

		/**
			* Triangle datastructure: each vertex has a corresponding opposite face.
			*/
		struct Face {
			std::array<Vertex *, 3> Vertices;        // Vertices of the triangle provided in CW order
			std::array<Face *, 3> AdjacentFaces;     // Each adjacent face opposite of the vertex provided
			bool IsDegenerate;
			uint8 NumVertices;

			/** Creates a degenerate face */
			Face(Vertex * const v1, Vertex * const v2) : Face(v1, v2, NULL) {}

			Face(Vertex * const v1, Vertex * const v2, Vertex * const v3) :
				Vertices({{ v1, v2, v3 }}),
				AdjacentFaces({{ this, this, this }}),
				IsDegenerate(v3 == NULL),
				NumVertices(v3 == NULL ? 2 : 3)
			{}

			int8 FindVertex(Vertex * const vertex) const {
				for (int8 i = NumVertices - 1; i >= 0; i--)
					if (Vertices[i] == vertex) return i;
				return -1;
			}

			int8 FindFace(Face * const face) const {
				for (int8 i = NumVertices - 1; i >= 0; i--)
					if (AdjacentFaces[i] == face) return i;
				return -1;
			}

			Face * GetAdjacentFaceCW(Vertex * const sharedVertex) {
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

			Face * GetAdjacentFaceCCW(Vertex * const sharedVertex) {
				int8 found = FindVertex(sharedVertex);
				// Shared vertex doesn't actually exist in this face
				if (found == -1) return NULL;
				return AdjacentFaces[GetCCWVertexIndex(found)];
			}

			/**
				* Returns a pair containing the position of the circumcircle center and the radius
				* of the circumcircle.
				*/
			Circle2D GetCircumcircle() const {
				if (IsDegenerate) return Circle2D({ 0, 0 }, 0.0);
				return CalculateCircumcircle(
					Vertices[0]->Point, Vertices[1]->Point, Vertices[2]->Point);
			}

			inline uint8 GetCWVertexIndex(const uint8 current) const {
				return (current + 1) % NumVertices;
			}

			inline int8 GetCWVertexIndex(Vertex * const current) const {
				auto value = FindVertex(current);
				return value == -1 ? -1 : GetCWVertexIndex(value);
			}

			inline Vertex * GetCWVertex(Vertex * const current) const {
				auto value = GetCWVertexIndex(current);
				return value == -1 ? NULL : Vertices[value];
			}

			inline uint8 GetCCWVertexIndex(const uint8 current) const {
				// If degenerate, need to return the opposite vertex index instead
				return (current + (IsDegenerate ? 1 : 2)) % NumVertices;
			}

			inline int8 GetCCWVertexIndex(Vertex * const current) const {
				auto value = FindVertex(current);
				return value == -1 ? -1 : GetCCWVertexIndex(value);
			}

			inline Vertex * GetCCWVertex(Vertex * const current) const {
				auto value = GetCCWVertexIndex(current);
				return value == -1 ? NULL : Vertices[value];
			}
		};

		class ConvexHull {
		private:
			// Convex hull vertices stored in CW winding order
			std::vector<Vertex *> HullVertices;
		public:
			inline Vertex * operator [] (const uint64 index) { return HullVertices[index]; }
			inline Vertex * const operator [] (const uint64 index) const {
				return HullVertices[index];
			}
			inline ConvexHull & operator = (const ConvexHull & other) {
				HullVertices = other.HullVertices;
				return *this;
			}
			inline void AddVertex(Vertex * const vert) { HullVertices.push_back(vert); }
			inline uint64 Size() const { return HullVertices.size(); }

			int64 MinIndex(std::function<double (Vertex * const)> valueOf) const;

			int64 LeftVertexIndex() const;
			int64 RightVertexIndex() const;
			int64 TopVertexIndex() const;
			int64 BottomVertexIndex() const;
		};

		class DelaunayGraph {
		private:
			std::unordered_set<Vertex *> Vertices;
			std::unordered_set<Face *> Faces;

			std::pair<Face *, int8> AdjustNewFaceAdjacencies(
				Face * const newFace, const uint8 pivotIndex);
			void RemoveFace(Face * const face, const uint8 pivotIndex);

		public:
			ConvexHull ConvexHull;

			~DelaunayGraph() {
				for (auto it : Vertices) delete it;
				for (auto it : Faces) delete it;
				Vertices.clear();
				Faces.clear();
			}

			inline uint64 VertexCount() const { return Vertices.size(); }
			inline uint64 FaceCount() const { return Faces.size(); }

			const std::vector<Vertex const *> GetVertices() const;
			const std::vector<Face const *> GetFaces() const;
			const std::vector<Edge> GetUniqueEdges() const;

			Face * FindFace(Vertex * const v1, Vertex * const v2);

			Vertex * AddVertex(Vertex * const vertex);
			Face * AddFace(Vertex * const v1, Vertex * const v2);
			Face * AddFace(Vertex * const v1, Vertex * const v2, Vertex * const v3);
			bool RemoveFace(Face * const face);
		};

		/**
		 * Returns 1: CW, 0: Colinear, -1: CCW
		 */
		inline int8 IsCWWinding(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			return FindWinding(v1->Point, v2->Point, v3->Point);
		}
	}
}

namespace std {
	template <>
	struct hash<utils::delaunay::Edge> {
		size_t operator()(const utils::delaunay::Edge & e) const {
			int64 seed = 0;
			std::hashCombine(seed, e.Start->VertexId);
			std::hashCombine(seed, e.End->VertexId);
			return seed;
		}
	};
}

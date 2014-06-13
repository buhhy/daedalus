#pragma once

#include "Engine.h"
#include "DataStructures.h"
#include <vector>
#include <array>
#include <unordered_set>

namespace utils {
	/**
	 * Basic divide and conquer algorithm and data structure taken from here:
	 * http://www.geom.uiuc.edu/~samuelp/del_project.html
	 */

	namespace delaunay {
		struct Face;
		struct Vertex;

		bool IsCWWinding(Vertex * const v1, Vertex * const v2, Vertex * const v3);

		struct Vertex {
			Vector2<> Point;
			Face * IncidentFace;
			uint64 VertexId;

			Vertex(const Vector2<> & point, uint64 id) :
				Point(point), IncidentFace(NULL), VertexId(id)
			{}

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
		private:
			uint8 NumVertices;

		public:
			std::array<Vertex *, 3> Vertices;        // Vertices of the triangle provided in CW order
			std::array<Face *, 3> AdjacentFaces;     // Each adjacent face opposite of the vertex provided
			bool IsDegenerate;

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

			Face * GetAdjacentFaceCCW(Vertex * const sharedVertex) {
				int8 found = FindVertex(sharedVertex);
				// Shared vertex doesn't actually exist in this face
				if (found == -1) return NULL;
				return AdjacentFaces[GetCCWVertexIndex(found)];
			}

			inline uint8 GetCWVertexIndex(const uint8 current) const {
				return (current + 1) % NumVertices;
			}

			inline uint8 GetCCWVertexIndex(const uint8 current) const {
				// If degenerate, need to return the opposite vertex index instead
				return (current + (IsDegenerate ? 1 : 2)) % NumVertices;
			}
		};

		class DelaunayGraph {
		private:
			std::unordered_set<Vertex *> Vertices;
			std::unordered_set<Face *> Faces;

			std::pair<Face *, int8> AdjustNewFaceAdjacencies(
				Face * const newFace, const uint8 currentIndex);

		public:
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

			Vertex * AddVertex(Vertex * const vertex);
			Face * CreateFace(Vertex * const v1, Vertex * const v2);
			Face * CreateFace(Vertex * const v1, Vertex * const v2, Vertex * const v3);
		};
	}

	void BuildDelaunay2D(
		delaunay::DelaunayGraph & graph,
		const std::vector<Vector2<> > & inputPoints);
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

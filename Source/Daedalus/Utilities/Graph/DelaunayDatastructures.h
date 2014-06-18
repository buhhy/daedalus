#pragma once

#include "Engine.h"
#include "DataStructures.h"

#include <array>
#include <deque>
#include <unordered_set>
#include <functional>

namespace utils {
	namespace delaunay {
		class Face;
		class ConvexHull;

		class Vertex {
		private:
			uint64 NumFaces;
			uint64 Id;

		public:
			Vector2<> Point;
			Face * IncidentFace;

			Vertex(const Vector2<> & point, uint64 id) :
				Point(point), IncidentFace(NULL), Id(id), NumFaces(0)
			{}

			uint64 AddFace(Face * const face);
			uint64 RemoveFace(Face * const face);

			inline uint64 VertexId() const { return Id; };
			inline uint64 FaceCount() const { return NumFaces; }

			inline bool operator == (const Vertex & other) const {
				return other.VertexId() == VertexId();
			}
		};

		struct Edge {
			Vertex * Start;
			Vertex * End;

			Edge(Vertex * const start, Vertex * const end) {
				if (start->VertexId() < end->VertexId()) {
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
		class Face {
		private:
			bool bIsDegenerate;
			uint8 NumVertices;
			uint64 Id;

		public:
			std::array<Vertex *, 3> Vertices;        // Vertices of the triangle provided in CW order
			std::array<Face *, 3> AdjacentFaces;     // Each adjacent face opposite of the vertex provided

			inline bool IsDegenerate() const { return bIsDegenerate; }
			inline uint8 VertexCount() const { return NumVertices; }
			inline uint64 FaceId() const { return Id; }

			/** Creates a degenerate face */
			Face(Vertex * const v1, Vertex * const v2, const uint64 id) : Face(v1, v2, NULL, id) {}

			Face(Vertex * const v1, Vertex * const v2, Vertex * const v3, const uint64 id) :
				Vertices({{ v1, v2, v3 }}),
				AdjacentFaces({{ this, this, this }}),
				bIsDegenerate(v3 == NULL),
				NumVertices(v3 == NULL ? 2 : 3),
				Id(id)
			{}

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
				return (current + (IsDegenerate() ? 1 : 2)) % NumVertices;
			}

			inline int8 GetCCWVertexIndex(Vertex * const current) const {
				auto value = FindVertex(current);
				return value == -1 ? -1 : GetCCWVertexIndex(value);
			}

			inline Vertex * GetCCWVertex(Vertex * const current) const {
				auto value = GetCCWVertexIndex(current);
				return value == -1 ? NULL : Vertices[value];
			}

			int8 FindVertex(Vertex * const vertex) const;
			int8 FindFace(Face * const face) const;
			Face * GetAdjacentFaceCW(Vertex * const sharedVertex);
			Face * GetAdjacentFaceCCW(Vertex * const sharedVertex);

			/**
			 * Returns a pair containing the position of the circumcircle center and the radius
			 * of the circumcircle.
			 */
			Circle2D GetCircumcircle() const;
		};

		class ConvexHull {
		private:
			// Convex hull vertices stored in CW winding order
			std::vector<Vertex *> HullVertices;
			bool bIsCollinear;

			uint64 GetSequence(
				std::deque<Vertex *> & deque,
				const uint64 start, const uint64 end,
				const int8 direction) const;
		public:
			ConvexHull() : bIsCollinear(true) {}

			inline Vertex * operator [] (const uint64 index) { return HullVertices[index]; }
			inline Vertex * const operator [] (const uint64 index) const {
				return HullVertices[index];
			}
			inline ConvexHull & operator = (const ConvexHull & other) {
				HullVertices = other.HullVertices;
				return *this;
			}
			inline uint64 Size() const { return HullVertices.size(); }
			
			/**
			 * Adds a new point in between the first point and the last point, returns true
			 * if the new point is collinear.
			 */
			bool AddVertex(Vertex * const vert);
			int64 MinIndex(std::function<double (Vertex * const)> valueOf) const;

			int64 LeftVertexIndex() const;
			int64 RightVertexIndex() const;
			int64 TopVertexIndex() const;
			int64 BottomVertexIndex() const;

			uint64 GetSequenceCW(
				std::deque<Vertex *> & deque,
				const uint64 start, const uint64 end
			) const;

			uint64 GetSequenceCCW(
				std::deque<Vertex *> & deque,
				const uint64 start, const uint64 end
			) const;
		};

		/**
		 * Returns 1: CW, 0: Colinear, -1: CCW
		 */
		inline int8 IsCWWinding(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			return FindWinding(v1->Point, v2->Point, v3->Point);
		}
	}

	class DelaunayGraph {
	private:
		std::unordered_set<delaunay::Vertex *> Vertices;
		std::unordered_set<delaunay::Face *> Faces;

		uint64 CurrentFaceId;
		uint64 CurrentVertexId;
		
		/**
		 * Adjusts the adjacency pointer of the new face to point to the closest CCW face
		 * around a provided pivot point. This method returns a pair that indicates which
		 * existing face needs to have which adjacency index updated to point to the new
		 * face.
		 */
		std::pair<delaunay::Face *, int8> AdjustNewFaceAdjacencies(
			delaunay::Face * const newFace, const uint8 pivotIndex);
		void RemoveFace(delaunay::Face * const face, const uint8 pivotIndex);
		uint64 GetNextFaceId();

	public:
		delaunay::ConvexHull ConvexHull;

		DelaunayGraph() : CurrentFaceId(0), CurrentVertexId(0) {}

		~DelaunayGraph() {
			for (auto it : Vertices) delete it;
			for (auto it : Faces) delete it;
			Vertices.clear();
			Faces.clear();
		}

		inline uint64 VertexCount() const { return Vertices.size(); }
		inline uint64 FaceCount() const { return Faces.size(); }

		const std::vector<delaunay::Vertex const *> GetVertices() const;
		const std::vector<delaunay::Face const *> GetFaces() const;
		const std::vector<delaunay::Edge> GetUniqueEdges() const;

		delaunay::Face * FindFace(delaunay::Vertex * const v1, delaunay::Vertex * const v2);

		delaunay::Vertex * AddVertex(delaunay::Vertex * const vertex);
		delaunay::Face * AddFace(delaunay::Vertex * const v1, delaunay::Vertex * const v2);

		/**
		 * Indices of vertices should be provided in CW winding.
		 */
		delaunay::Face * AddFace(
			delaunay::Vertex * const v1,
			delaunay::Vertex * const v2,
			delaunay::Vertex * const v3);
		bool RemoveFace(delaunay::Face * const face);
	};
}

namespace std {
	template <>
	struct hash<utils::delaunay::Edge> {
		size_t operator()(const utils::delaunay::Edge & e) const {
			int64 seed = 0;
			std::hashCombine(seed, e.Start->VertexId());
			std::hashCombine(seed, e.End->VertexId());
			return seed;
		}
	};
}

#pragma once

#include "Engine.h"
#include "DataStructures.h"

#include <array>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace utils {
	namespace delaunay {
		class Face;
		class ConvexHull;

		class Vertex {
		private:
			uint64 NumFaces;
			uint64 Id;
			Vector2<> Point;
			

			Vertex(const Vector2<> & point, uint64 id, Face * inf, uint64 numFaces) :
				Point(point), IncidentFace(inf), Id(id), NumFaces(numFaces)
			{}

		public:
			Face * IncidentFace;

			Vertex(const Vector2<> & point, uint64 id) :
				Vertex(point, id, NULL, 0)
			{}
			Vertex(const Vertex & copy) : Vertex(copy.Point, copy.Id, NULL, copy.NumFaces) {}

			uint64 AddFace(Face * const face);
			uint64 RemoveFace(Face * const face);

			inline Vector2<> GetPoint() const { return Point; }
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
				AdjacentFaces({{ this, this, v3 == NULL ? NULL : this }}),
				bIsDegenerate(v3 == NULL),
				NumVertices(v3 == NULL ? 2 : 3),
				Id(id)
			{}

			Face(const Face & copy) : Face(copy.Vertices[0], copy.Vertices[1], copy.Vertices[2], copy.Id) {}

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
				const bool isCw) const;
		public:
			ConvexHull() : bIsCollinear(false) {}
			ConvexHull(std::vector<Vertex *> hullVertices) : HullVertices(hullVertices) {}

			inline std::vector<Vertex *>::const_iterator CBegin() const {
				return HullVertices.cbegin();
			}
			inline std::vector<Vertex *>::const_iterator CEnd() const {
				return HullVertices.cend();
			}

			inline Vertex * operator [] (const uint64 index) { return HullVertices[index]; }
			inline Vertex * const operator [] (const uint64 index) const {
				return HullVertices[index];
			}

			inline ConvexHull & operator = (const ConvexHull & other) {
				HullVertices = other.HullVertices;
				return *this;
			}

			inline ConvexHull & operator = (const std::vector<Vertex *> & other) {
				HullVertices = other;
				return *this;
			}

			inline uint64 Size() const { return HullVertices.size(); }

			inline uint64 GetSequenceCW(
				std::deque<Vertex *> & deque,
				const uint64 start, const uint64 end
			) const { return GetSequence(deque, start, end, true); }

			inline uint64 GetSequenceCCW(
				std::deque<Vertex *> & deque,
				const uint64 start, const uint64 end
			) const { return GetSequence(deque, start, end, false); }

			inline uint64 NextIndex(const uint64 current) const {
				return (current + 1) % Size();
			}

			inline uint64 PrevIndex(const uint64 current) const {
				if (current < 1)
					return Size() - 1;
				return current - 1;
			}
			
			/**
			 * Adds a new point in between the first point and the last point, returns true
			 * if the new point is collinear.
			 */
			bool AddVertex(Vertex * const vert);
			int64 FindVertexById(const uint64 id) const;
			int64 MinIndex(std::function<double (Vertex * const)> valueOf) const;

			int64 LeftVertexIndex() const;
			int64 RightVertexIndex() const;
			int64 TopVertexIndex() const;
			int64 BottomVertexIndex() const;

			uint64 GetRange(const uint64 start, const uint64 end, const bool isCW) const;
		};

		/**
		 * Returns 1: CW, 0: Colinear, -1: CCW
		 */
		inline int8 IsCWWinding(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			return FindWinding(v1->GetPoint(), v2->GetPoint(), v3->GetPoint());
		}
	}

	class DelaunayGraph {
	private:
		std::unordered_set<delaunay::Vertex *> Vertices;
		std::unordered_set<delaunay::Face *> Faces;

		// These hashmaps are used for fast lookup of vertices and faces by ID, they should
		// be kept up-to-date with the vertex and face lists.
		std::unordered_map<uint64, delaunay::Vertex *> IdVertexMap;
		std::unordered_map<uint64, delaunay::Face *> IdFaceMap;

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
		void AdjustRemovedFaceAdjacencies(delaunay::Face * const face, const uint8 pivotIndex);
		uint64 GetNextFaceId();

	public:
		delaunay::ConvexHull ConvexHull;

		DelaunayGraph() : CurrentFaceId(0), CurrentVertexId(0) {}
		
		DelaunayGraph(const DelaunayGraph & copy);

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

		/**
		 * Adds a pre-created vertex. The ID should be provide and be unique. The current
		 * vertex ID will be updated to avoid duplicate IDs.
		 * TODO: perhaps this should be made private
		 */
		delaunay::Vertex * AddVertex(delaunay::Vertex * const vertex);

		/**
		 * Adds a pre-created floating face. The ID should be provide and be unique. The current
		 * face ID will be updated to avoid duplicate IDs. Unlike the other methods for adding
		 * faces, this method will not update adjacencies and is more meant for initialization.
		 * TODO: perhaps this should be made private
		 */
		delaunay::Face * AddFace(delaunay::Face * const face);
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

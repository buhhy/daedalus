#pragma once

#include <Utilities/DataStructures.h>

#include <array>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <functional>

/**
 * Typedefs for hash.
 */
namespace utils {
	namespace delaunay {
		typedef Vector2<int64_t> DelaunayId;                   // Positional ID
		typedef std::pair<DelaunayId, uint64_t> GhostId;       // Position & local ID aggregate
	}
}

namespace std {
	template <>
	struct hash<utils::delaunay::GhostId> {
		size_t operator()(const utils::delaunay::GhostId & g) const {
			int64_t seed = 0;
			std::hashCombine(seed, hash<utils::Vector2<int64_t>>()(g.first));
			std::hashCombine(seed, g.second);
			return (unsigned) seed;
		}
	};
}

/**
 * Data structures for a tileable infinite Delaunay triangulation.
 */
namespace utils {
	namespace delaunay {
		class Face;
		class ConvexHull;

		class Vertex {
		private:
			uint64_t NumFaces;
			uint64_t Id;
			uint64_t ForeignId;      // Local ID of the vertex in the foreign graph
			Vector2<> Point;
			bool bIsForeign;
			
			DelaunayId GraphOffset;
			Face * IncidentFace;

			Vertex(
				const DelaunayId & gid, const Vector2<> & point,
				const uint64_t id, const uint64_t fid, bool isForeign,
				Face * const inf, const uint64_t numFaces
			) : GraphOffset(gid), Point(point), IncidentFace(inf),
				Id(id), ForeignId(fid), bIsForeign(isForeign), NumFaces(numFaces)
			{}

		public:
			Vertex(const DelaunayId & gid, const Vector2<> & point, const uint64_t id) :
				Vertex(gid, point, id, 0, false, NULL, 0)
			{}
			Vertex(
				const DelaunayId & gid, const Vector2<> & point,
				const uint64_t id, const uint64_t fid
			) : Vertex(gid, point, id, fid, true, NULL, 0)
			{}
			Vertex(const Vertex & copy) :
				Vertex(copy.GraphOffset, copy.Point, copy.Id, copy.ForeignId,
					copy.bIsForeign, NULL, copy.NumFaces)
			{}

			uint64_t AddFace(Face * const face);
			uint64_t RemoveFace(Face * const face);

			inline Face * const GetIncidentFace() const { return IncidentFace; }
			inline const DelaunayId & ParentGraphOffset() const { return GraphOffset; }
			inline const Vector2<> & GetPoint() const { return Point; }
			inline uint64_t VertexId() const { return Id; };
			inline uint64_t ForeignVertexId() const { return ForeignId; }
			inline uint64_t FaceCount() const { return NumFaces; }
			inline bool IsForeign() const { return bIsForeign; }

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
			uint8_t NumVertices;
			uint64_t Id;

		public:
			std::array<Vertex *, 3> Vertices;        // Vertices of the triangle provided in CW order
			std::array<Face *, 3> AdjacentFaces;     // Each adjacent face opposite of the vertex provided

			inline bool IsDegenerate() const { return bIsDegenerate; }
			inline uint8_t VertexCount() const { return NumVertices; }
			inline uint64_t FaceId() const { return Id; }

			/** Creates a degenerate face */
			Face(Vertex * const v1, Vertex * const v2, const uint64_t id) :
				Face(v1, v2, NULL, id)
			{}

			Face(Vertex * const v1, Vertex * const v2, Vertex * const v3, const uint64_t id) :
				Vertices({{ v1, v2, v3 }}),
				AdjacentFaces({{ this, this, v3 == NULL ? NULL : this }}),
				bIsDegenerate(v3 == NULL),
				NumVertices(v3 == NULL ? 2 : 3),
				Id(id)
			{}

			Face(const Face & copy) : Face(copy.Vertices[0], copy.Vertices[1], copy.Vertices[2], copy.Id) {}

			inline uint8_t GetCWVertexIndex(const uint8_t current) const {
				return (current + 1) % NumVertices;
			}

			inline int8_t GetCWVertexIndex(Vertex * const current) const {
				auto value = FindVertex(current);
				return value == -1 ? -1 : GetCWVertexIndex(value);
			}

			inline Vertex * GetCWVertex(Vertex * const current) const {
				auto value = GetCWVertexIndex(current);
				return value == -1 ? NULL : Vertices[value];
			}

			inline uint8_t GetCCWVertexIndex(const uint8_t current) const {
				// If degenerate, need to return the opposite vertex index instead
				return (current + (IsDegenerate() ? 1 : 2)) % NumVertices;
			}

			inline int8_t GetCCWVertexIndex(Vertex * const current) const {
				auto value = FindVertex(current);
				return value == -1 ? -1 : GetCCWVertexIndex(value);
			}

			inline Vertex * GetCCWVertex(Vertex * const current) const {
				auto value = GetCCWVertexIndex(current);
				return value == -1 ? NULL : Vertices[value];
			}

			int8_t FindVertex(Vertex * const vertex) const;
			int8_t FindFace(Face * const face) const;
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

			uint32_t GetSequence(
				std::deque<Vertex *> & deque,
				const uint32_t start, const uint32_t end,
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

			inline Vertex * operator [] (const uint32_t index) { return HullVertices[index]; }
			inline Vertex * const operator [] (const uint32_t index) const {
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

			inline uint32_t Size() const { return HullVertices.size(); }

			inline uint32_t GetSequenceCW(
				std::deque<Vertex *> & deque,
				const uint32_t start, const uint32_t end
			) const { return GetSequence(deque, start, end, true); }

			inline uint32_t GetSequenceCCW(
				std::deque<Vertex *> & deque,
				const uint32_t start, const uint32_t end
			) const { return GetSequence(deque, start, end, false); }

			inline uint32_t NextIndex(const uint32_t current) const {
				return (current + 1) % Size();
			}

			inline uint32_t PrevIndex(const uint32_t current) const {
				if (current < 1)
					return Size() - 1;
				return current - 1;
			}
			
			/**
			 * Adds a new point in between the first point and the last point, returns true
			 * if the new point is collinear.
			 */
			bool AddVertex(Vertex * const vert);
			int32_t FindVertexById(const uint32_t id) const;
			uint32_t MinIndex(const std::function<double (Vertex * const)> & valueOf) const;

			uint32_t LeftVertexIndex() const;
			uint32_t RightVertexIndex() const;
			uint32_t TopVertexIndex() const;
			uint32_t BottomVertexIndex() const;

			uint32_t GetRange(const uint32_t start, const uint32_t end, const bool isCW) const;
		};

		/**
		 * Returns 1: CW, 0: Colinear, -1: CCW
		 */
		inline int8_t IsCWWinding(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			return FindWinding(v1->GetPoint(), v2->GetPoint(), v3->GetPoint());
		}
	}

	class DelaunayGraph {
	private:
		std::unordered_set<delaunay::Vertex *> Vertices;
		std::unordered_set<delaunay::Face *> Faces;

		// These hashmaps are used for fast lookup of vertices and faces by ID, they should
		// be kept up-to-date with the vertex and face lists.
		std::unordered_map<uint64_t, delaunay::Vertex *> IdVertexMap;
		std::unordered_map<uint64_t, delaunay::Face *> IdFaceMap;

		uint64_t CurrentFaceId;
		uint64_t CurrentVertexId;

		delaunay::DelaunayId Offset;
		
		/**
		 * Adjusts the adjacency pointer of the new face to point to the closest CCW face
		 * around a provided pivot point. This method returns a pair that indicates which
		 * existing face needs to have which adjacency index updated to point to the new
		 * face.
		 */
		std::pair<delaunay::Face *, int8_t> AdjustNewFaceAdjacencies(
			delaunay::Face * const newFace, const uint8_t pivotIndex);
		void AdjustRemovedFaceAdjacencies(delaunay::Face * const face, const uint8_t pivotIndex);
		uint64_t GetNextFaceId();
		uint64_t GetNextVertexId();

		/**
		 * Adds a pre-created vertex. The ID should be provide and be unique. The current
		 * vertex ID will be updated to avoid duplicate IDs.
		 */
		delaunay::Vertex * AddVertex(delaunay::Vertex * const vertex);

		/**
		 * Adds the provided vertex as a ghost vertex into the current graph. This means
		 * duplicating the other vertex with this graph as the new owner.
		 * @param vertex Vertex to add, foreign vertices will be duplicated.
		 */
		delaunay::Vertex * AddGhostVertex(delaunay::Vertex * const vertex);

		/**
		 * Adds a pre-created floating face. The ID should be provide and be unique. The current
		 * face ID will be updated to avoid duplicate IDs. Unlike the other methods for adding
		 * faces, this method will not update adjacencies and is more meant for initialization.
		 */
		delaunay::Face * AddFace(delaunay::Face * const face);

	public:
		delaunay::ConvexHull ConvexHull;

		DelaunayGraph(const delaunay::DelaunayId id) :
			CurrentFaceId(0), CurrentVertexId(0), Offset(id) {}
		
		DelaunayGraph(const DelaunayGraph & copy);

		~DelaunayGraph() {
			for (auto it : Vertices) delete it;
			for (auto it : Faces) delete it;
			Vertices.clear();
			Faces.clear();
		}

		inline uint32_t VertexCount() const { return Vertices.size(); }
		inline uint32_t FaceCount() const { return Faces.size(); }
		inline delaunay::DelaunayId GraphOffset() const { return Offset; }

		const std::vector<delaunay::Vertex const *> GetVertices() const;
		const std::vector<delaunay::Face const *> GetFaces() const;
		const std::vector<delaunay::Edge> GetUniqueEdges() const;

		delaunay::Vertex * AddVertex(const Vector2<> & point, const uint64_t id);

		delaunay::Face * FindFace(delaunay::Vertex * const v1, delaunay::Vertex * const v2);
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
			int64_t seed = 0;
			std::hashCombine(seed, e.Start->VertexId());
			std::hashCombine(seed, e.End->VertexId());
			return (unsigned) seed;
		}
	};
}

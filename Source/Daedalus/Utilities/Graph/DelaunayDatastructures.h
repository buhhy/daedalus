#pragma once

#include <Utilities/Algebra/Algebra2.h>

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
		typedef Vector2<Int64> DelaunayId;                   // Positional ID
		typedef std::pair<DelaunayId, Uint64> GhostId;       // Position & local ID aggregate
	}
}

namespace std {
	template <>
	struct hash<utils::delaunay::GhostId> {
		size_t operator()(const utils::delaunay::GhostId & g) const {
			Int64 seed = 0;
			std::hashCombine(seed, hash<utils::Vector2<Int64>>()(g.first));
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

		typedef std::unordered_map<Uint64, Face * const> IdFaceMap;



		/**
		 * Vertex datastructure
		 */
		class Vertex {
		private:
			IdFaceMap IncidentFaces;
			Uint64 Id;
			Uint64 ForeignId;      // Local ID of the vertex in the foreign graph
			Vector2<> Point;
			bool bIsForeign;
			
			DelaunayId GraphOffset;

			// TODO: update copy constructor for face map
			Vertex(
				const DelaunayId & gid, const Vector2<> & point,
				const Uint64 id, const Uint64 fid, bool isForeign
			) : GraphOffset(gid), Point(point),
				Id(id), ForeignId(fid), bIsForeign(isForeign)
			{}

		public:
			Vertex(const DelaunayId & gid, const Vector2<> & point, const Uint64 id) :
				Vertex(gid, point, id, 0, false)
			{}
			Vertex(
				const DelaunayId & gid, const Vector2<> & point,
				const Uint64 id, const Uint64 fid
			) : Vertex(gid, point, id, fid, true)
			{}
			Vertex(const Vertex & copy) :
				Vertex(copy.GraphOffset, copy.Point, copy.Id, copy.ForeignId, copy.bIsForeign)
			{}

			Uint64 AddFace(Face * const face);
			Uint64 RemoveFace(Face * const face);

			/**
			 * @return true if the current vertex is not part of the concave/convex hull
			 *              formed by the faces that are incident to this vertex
			 *         false if the current vertex is not part of that concave/convex hull
			 */
			bool IsSurrounded() const;

			inline const IdFaceMap & GetIncidentFaces() const { return IncidentFaces; }
			inline Face * const GetFirstIncidentFace() const {
				if (FaceCount() == 0)
					return NULL;
				return IncidentFaces.cbegin()->second;
			}
			inline const DelaunayId & ParentGraphOffset() const { return GraphOffset; }
			inline const Vector2<> & GetPoint() const { return Point; }
			inline Uint64 VertexId() const { return Id; };
			inline Uint64 ForeignVertexId() const { return ForeignId; }
			inline Uint64 FaceCount() const { return IncidentFaces.size(); }
			inline bool IsForeign() const { return bIsForeign; }
		};




		/**
		 * Edge datastructure - this is not actually used in the Delaunay triangulation
		 * process, this edge structure is used for convenient transfer of edge information
		 * such as a method return.
		 */
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
		};




		/**
		 * Triangle datastructure: each vertex has a corresponding opposite face.
		 */
		class Face {
		private:
			bool bIsDegenerate;
			Uint8 NumVertices;
			Uint64 Id;

		public:
			std::array<Vertex *, 3> Vertices;        // Vertices of the triangle provided in CW order
			std::array<Face *, 3> AdjacentFaces;     // Each adjacent face opposite of the vertex provided

			inline bool IsDegenerate() const { return bIsDegenerate; }
			inline Uint8 VertexCount() const { return NumVertices; }
			inline Uint64 FaceId() const { return Id; }

			/** Creates a degenerate face */
			Face(Vertex * const v1, Vertex * const v2, const Uint64 id) :
				Face(v1, v2, NULL, id)
			{}

			Face(Vertex * const v1, Vertex * const v2, Vertex * const v3, const Uint64 id) :
				Vertices({{ v1, v2, v3 }}),
				AdjacentFaces({{ this, this, v3 == NULL ? NULL : this }}),
				bIsDegenerate(v3 == NULL),
				NumVertices(v3 == NULL ? 2 : 3),
				Id(id)
			{}

			Face(const Face & copy) : Face(copy.Vertices[0], copy.Vertices[1], copy.Vertices[2], copy.Id) {}

			inline Uint8 GetCWVertexIndex(const Uint8 current) const {
				return (current + 1) % NumVertices;
			}

			inline Int8 GetCWVertexIndex(Vertex const * const current) const {
				auto value = FindVertex(current);
				return value == -1 ? -1 : GetCWVertexIndex(value);
			}

			inline Vertex * GetCWVertex(Vertex const * const current) const {
				auto value = GetCWVertexIndex(current);
				return value == -1 ? NULL : Vertices[value];
			}

			inline Uint8 GetCCWVertexIndex(const Uint8 current) const {
				// If degenerate, need to return the opposite vertex index instead
				return (current + (IsDegenerate() ? 1 : 2)) % NumVertices;
			}

			inline Int8 GetCCWVertexIndex(Vertex const * const current) const {
				auto value = FindVertex(current);
				return value == -1 ? -1 : GetCCWVertexIndex(value);
			}

			inline Vertex * GetCCWVertex(Vertex const * const current) const {
				auto value = GetCCWVertexIndex(current);
				return value == -1 ? NULL : Vertices[value];
			}

			Int8 FindVertex(Vertex const * const vertex) const;
			Int8 FindFace(Face const * const face) const;
			Face * GetAdjacentFaceCW(Vertex const * const sharedVertex);
			Face * GetAdjacentFaceCCW(Vertex const * const sharedVertex);

			/**
			 * Returns a pair containing the position of the circumcircle center and the radius
			 * of the circumcircle.
			 */
			Circle2D GetCircumcircle() const;
		};




		/**
		 * Comparison operators
		 */
		inline bool operator == (const Vertex & left, const Vertex & right) {
			return left.VertexId() == right.VertexId();
		}

		inline bool operator == (const Face & left, const Face & right) {
			return left.FaceId() == right.FaceId();
		}

		inline bool operator == (const Edge & left, const Edge & right) {
			return left.Start == right.Start && left.End == right.End;
		}

		inline bool operator != (const Vertex & left, const Vertex & right) {
			return left.VertexId() != right.VertexId();
		}

		inline bool operator != (const Face & left, const Face & right) {
			return left.FaceId() != right.FaceId();
		}

		inline bool operator != (const Edge & left, const Edge & right) {
			return left.Start != right.Start && left.End != right.End;
		}




		/**
		 * Convex hull datastructure - the vertices of the convex hull are stored in clockwise
		 * order, the last vertex will always lead to an edge back to the first.
		 */
		class ConvexHull {
		private:
			typedef std::function<double (Vertex * const)> VertexValueExtractor;

			// Convex hull vertices stored in CW winding order
			std::vector<Vertex *> HullVertices;
			bool bIsCollinear;
			
			Uint32 MinIndex(const VertexValueExtractor & valueOf) const;
			Uint64 GetSequence(
				std::deque<Vertex *> & deque,
				const Uint64 start, const Uint64 end,
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

			inline Vertex * operator [] (const Uint64 index) { return HullVertices[index]; }
			inline Vertex * const operator [] (const Uint64 index) const {
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

			inline Uint64 Size() const { return HullVertices.size(); }

			inline Uint64 GetSequenceCW(
				std::deque<Vertex *> & deque,
				const Uint64 start, const Uint64 end
			) const { return GetSequence(deque, start, end, true); }

			inline Uint64 GetSequenceCCW(
				std::deque<Vertex *> & deque,
				const Uint64 start, const Uint64 end
			) const { return GetSequence(deque, start, end, false); }

			inline Uint64 NextIndex(const Uint64 current) const {
				return (current + 1) % Size();
			}

			inline Uint64 PrevIndex(const Uint64 current) const {
				if (current < 1)
					return Size() - 1;
				return current - 1;
			}
			
			/**
			 * Adds a new point in between the first point and the last point, returns true
			 * if the new point is collinear.
			 */
			bool AddVertex(Vertex * const vert);
			utils::Vector2<> Centroid() const;
			Int32 FindVertexById(const Uint64 id) const;

			Uint32 LeftVertexIndex() const;
			Uint32 RightVertexIndex() const;
			Uint32 ClosestVertexIndex(const utils::Vector2<> & compare) const;

			Uint64 GetRange(const Uint64 start, const Uint64 end, const bool isCW) const;
		};

		/**
		 * Returns 1: CW, 0: Colinear, -1: CCW
		 */
		inline Int8 IsCWWinding(Vertex * const v1, Vertex * const v2, Vertex * const v3) {
			return FindWinding(v1->GetPoint(), v2->GetPoint(), v3->GetPoint());
		}
	}




	/**
	 * An entire representation of a Delaunay triangulation, this structure is meant to be
	 * tileable and represents a single tile. It may contain references to foreign vertices
	 * from another tile.
	 */
	class DelaunayGraph {
	private:
		std::unordered_set<delaunay::Vertex *> Vertices;
		std::unordered_set<delaunay::Face *> Faces;

		// These hashmaps are used for fast lookup of vertices and faces by ID, they should
		// be kept up-to-date with the vertex and face lists.
		std::unordered_map<Uint64, delaunay::Vertex *> IdVertexMap;
		std::unordered_map<Uint64, delaunay::Face *> IdFaceMap;
		std::unordered_map<delaunay::GhostId, delaunay::Vertex *> ForeignIdVertexMap;

		Uint64 CurrentFaceId;
		Uint64 CurrentVertexId;

		delaunay::DelaunayId Offset;
		
		/**
		 * Adjusts the adjacency pointer of the new face to point to the closest CCW face
		 * around a provided pivot point. This method returns a pair that indicates which
		 * existing face needs to have which adjacency index updated to point to the new
		 * face.
		 */
		std::pair<delaunay::Face *, Int8> AdjustNewFaceAdjacencies(
			delaunay::Face * const newFace, const Uint8 pivotIndex);
		void AdjustRemovedFaceAdjacencies(delaunay::Face * const face, const Uint8 pivotIndex);
		Uint64 GetNextFaceId();
		Uint64 GetNextVertexId();

		/**
		 * Adds a pre-created vertex. The ID should be provide and be unique. The current
		 * vertex ID will be updated to avoid duplicate IDs.
		 */
		delaunay::Vertex * AddVertexToCache(delaunay::Vertex * const vertex);

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
		delaunay::Face * AddFaceToCache(delaunay::Face * const face);
		bool RemoveFaceFromCache(delaunay::Face * const face);

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

		inline Uint64 VertexCount() const { return Vertices.size(); }
		inline Uint64 FaceCount() const { return Faces.size(); }
		inline delaunay::DelaunayId GraphOffset() const { return Offset; }

		const std::vector<delaunay::Vertex const *> GetVertices() const;
		const std::vector<delaunay::Face const *> GetFaces() const;
		const std::vector<delaunay::Edge> GetUniqueEdges() const;

		delaunay::Vertex * AddVertex(const Vector2<> & point, const Uint64 id);
		delaunay::Face * AddFace(
			delaunay::Vertex * const v1, delaunay::Vertex * const v2);

		delaunay::Face * FindFace(
			delaunay::Vertex const * const v1, delaunay::Vertex const * const v2);

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
			Int64 seed = 0;
			std::hashCombine(seed, e.Start->VertexId());
			std::hashCombine(seed, e.End->VertexId());
			return (unsigned) seed;
		}
	};
}

#pragma once

#include <Utilities/DataStructures.h>
#include "DelaunayDatastructures.h"

#include <unordered_set>

namespace utils {
	namespace halfedge {
		class Edge;
		class Face;

		class Vertex {
		private:
			Vector2<> Point;
			uint64_t Id;
			uint64_t NumEdges;
			Edge * ExitEdge;

		public:
			const Vector2<> & GetPoint() const { return Point; }
			uint64_t VertexId() const { return Id; }
			uint64_t EdgeCount() const { return NumEdges; }
			Edge * GetExitEdge() { return ExitEdge; }
			
			uint64_t AddEdge(Edge * const edge);
			uint64_t RemoveEdge(Edge * const edge);

			Vertex(const Vector2<> & point, const uint64_t id): Point(point), Id(id) {}

		};

		/**
		 * http://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
		 */
		class Edge {
		private:
			uint64_t Id;

		public:
			Vertex * Origin;
			Face * EdgeFace;
			Edge * NextLeft;
			Edge * Twin;
			
			uint64_t VertexId() const { return Id; }

		};

		class Face {
		private:
			uint64_t Id;
			uint64_t NumEdges;
			Edge * BorderEdge;

		public:
			uint64_t FaceId() const { return Id; }
			uint64_t EdgeCount() const { return NumEdges; }
			Edge * GetBorderEdge() { return BorderEdge; }

			Face(const uint64_t id): Id(id) {}

		};
	}

	class VoronoiGraph {
	private:
		std::unordered_set<halfedge::Vertex *> Vertices;
		std::unordered_set<halfedge::Face *> Faces;
		std::unordered_set<halfedge::Edge *> Edges;

	public:
		VoronoiGraph(const DelaunayGraph & source);

		~VoronoiGraph() {
			for (auto it : Vertices) delete it;
			for (auto it : Faces) delete it;
			for (auto it : Edges) delete it;
			Vertices.clear();
			Faces.clear();
			Edges.clear();
		}
			
		inline uint64_t VertexCount() const { return Vertices.size(); }
		inline uint64_t FaceCount() const { return Faces.size(); }
		inline uint64_t EdgeCount() const { return Edges.size(); }
	};
}

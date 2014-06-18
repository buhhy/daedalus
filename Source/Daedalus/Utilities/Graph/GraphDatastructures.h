#pragma once

#include "Engine.h"
#include "Datastructures.h"
#include "DelaunayDatastructures.h"

#include <unordered_set>

namespace utils {
	namespace halfedge {
		class Edge;
		class Face;

		class Vertex {
		private:
			Vector2<> Point;
			uint64 Id;
			uint64 NumEdges;
			Edge * ExitEdge;

		public:
			const Vector2<> & GetPoint() const { return Point; }
			uint64 VertexId() const { return Id; }
			uint64 EdgeCount() const { return NumEdges; }
			Edge * GetExitEdge() { return ExitEdge; }
			
			uint64 AddEdge(Edge * const edge);
			uint64 RemoveEdge(Edge * const edge);

			Vertex(const Vector2<> & point, const uint64 id): Point(point), Id(id) {}

		};

		/**
		 * http://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
		 */
		class Edge {
		private:
			uint64 Id;

		public:
			Vertex * Origin;
			Face * EdgeFace;
			Edge * NextLeft;
			Edge * Twin;
			
			uint64 VertexId() const { return Id; }

		};

		class Face {
		private:
			uint64 Id;
			uint64 NumEdges;
			Edge * BorderEdge;

		public:
			uint64 FaceId() const { return Id; }
			uint64 EdgeCount() const { return NumEdges; }
			Edge * GetBorderEdge() { return BorderEdge; }

			Face(const uint64 id): Id(id) {}

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
			
		inline uint64 VertexCount() const { return Vertices.size(); }
		inline uint64 FaceCount() const { return Faces.size(); }
		inline uint64 EdgeCount() const { return Edges.size(); }
	};
}

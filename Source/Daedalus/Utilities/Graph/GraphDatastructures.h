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
			Uint64 Id;
			Uint64 NumEdges;
			Edge * ExitEdge;

		public:
			const Vector2<> & GetPoint() const { return Point; }
			Uint64 VertexId() const { return Id; }
			Uint64 EdgeCount() const { return NumEdges; }
			Edge * GetExitEdge() { return ExitEdge; }
			
			Uint64 AddEdge(Edge * const edge);
			Uint64 RemoveEdge(Edge * const edge);

			Vertex(const Vector2<> & point, const Uint64 id): Point(point), Id(id) {}

		};

		/**
		 * http://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
		 */
		class Edge {
		private:
			Uint64 Id;

		public:
			Vertex * Origin;
			Face * EdgeFace;
			Edge * NextLeft;
			Edge * Twin;
			
			Uint64 VertexId() const { return Id; }

		};

		class Face {
		private:
			Uint64 Id;
			Uint64 NumEdges;
			Edge * BorderEdge;

		public:
			Uint64 FaceId() const { return Id; }
			Uint64 EdgeCount() const { return NumEdges; }
			Edge * GetBorderEdge() { return BorderEdge; }

			Face(const Uint64 id): Id(id) {}

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
			
		inline Uint64 VertexCount() const { return Vertices.size(); }
		inline Uint64 FaceCount() const { return Faces.size(); }
		inline Uint64 EdgeCount() const { return Edges.size(); }
	};
}

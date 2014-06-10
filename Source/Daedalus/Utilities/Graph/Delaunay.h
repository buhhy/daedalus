#pragma once

#include "Engine.h"
#include "Vector2.h"
#include <vector>

namespace utils {
	/**
	 * Basic divide and conquer algorithm and data structure taken from here:
	 * http://www.geom.uiuc.edu/~samuelp/del_project.html
	 */

	namespace delaunay {
		struct Vertex;
		struct Face;

		/**
		 *                            |
		 *        Next left edge      |      Next right edge
		 * ---------------------------*-----------------------------
		 *                            | End vertex
		 *                            |
		 *                            |
		 *              o             |             o
		 *          Left face         |        Right face
		 *                            |
		 *                            | Start vertex
		 * ---------------------------*-----------------------------
		 *        Prev left edge      |     Prev right edge
		 *                            |
		 */
		struct Edge {
			Vertex * StartVertex;
			Vertex * EndVertex;
			Edge * NextLeftEdge;			// Next CW edge from end vertex
			Edge * NextRightEdge;			// Next CCW edge from end vertex
			Edge * PrevLeftEdge;			// Previous CCW edge from start vertex
			Edge * PrevRightEdge;			// Previous CW edge from start vertex
			Face * LeftFace;
			Face * RightFace;

			Edge(Vertex * const start, Vertex * const end) :
				StartVertex(start),
				EndVertex(end),
				NextLeftEdge(NULL),
				NextRightEdge(NULL),
				PrevLeftEdge(NULL),
				PrevRightEdge(NULL),
				LeftFace(NULL),
				RightFace(NULL) {}
		};

		struct Vertex {
			Vector2<double> Point;
			Edge * Edge;

			Vertex(const Vector2<double> & point) :
				Point(point), Edge(NULL) {}
		};

		struct Face {
			Edge * Edge;
		};

		struct DelaunayGraph {
			std::vector<Vertex *> Vertices;
			std::vector<Edge *> Edges;
			std::vector<Face *> Faces;

			~DelaunayGraph() {
				for (auto it : Vertices) delete it;
				for (auto it : Edges) delete it;
				for (auto it : Faces) delete it;
				Vertices.clear();
				Edges.clear();
				Faces.clear();
			}

			Vertex * CreateVertex(
				const Vector2<double> & point
			) {
				Vertex * newVertex = new Vertex(point);
				Vertices.push_back(newVertex);
				return newVertex;
			}

			Edge * CreateEdge(
				const uint64 startVertexIndex,
				const uint64 endVertexIndex
			) {
				Vertex * const v1 = Vertices[startVertexIndex];
				Vertex * const v2 = Vertices[endVertexIndex];
				Edge * newEdge = new Edge(v1, v2);
				Edges.push_back(newEdge);
				v1->Edge = v2->Edge = newEdge;
				return newEdge;
			}

			/**
			 * Indices of vertices should be provided in CW winding.
			 */
			Face * CreateTriangle(const uint64 i1, const uint64 i2, const uint64 i3) {
				Edge * e1 = CreateEdge(i1, i2);
				Edge * e2 = CreateEdge(i2, i3);
				Edge * e3 = CreateEdge(i3, i1);
				Face * face = new Face();

				e1->NextLeftEdge = e1->NextRightEdge = e2;
				e2->NextLeftEdge = e2->NextRightEdge = e3;
				e3->NextLeftEdge = e3->NextRightEdge = e1;

				e1->PrevLeftEdge = e1->PrevRightEdge = e3;
				e2->PrevLeftEdge = e2->PrevRightEdge = e1;
				e3->PrevLeftEdge = e3->PrevRightEdge = e2;

				e1->RightFace = e2->RightFace = e3->RightFace = face;

				face->Edge = e1;

				return face;
			}
		};
	}

	delaunay::DelaunayGraph BuildDelaunay2D(
		std::vector<double> & results,
		const std::vector<Vector2<double> *> & inputPoints);
}

#pragma once

#include "GraphDatastructures.h"

#include <vector>

namespace utils {
	class IDelaunayDAC2DDebug;

	/**
	 * Basic divide and conquer algorithm and data structure taken from here:
	 * http://www.geom.uiuc.edu/~samuelp/del_project.html. My implementation will handle
	 * merging tileable graphs without consolidating them into a single graph.
	 */
	class DelaunayBuilderDAC2D {
	public:
		using InputPointList = std::vector<std::pair<utils::Vector2<>, Uint64>>;
		using GraphHullIndexArray = std::array<std::pair<DelaunayGraph *, Uint32>, 4>;
		using AddedFaceList = std::vector<std::array<delaunay::Vertex *, 3>>;

		using VertexComparator =
			std::function<bool (delaunay::Vertex * const p1, delaunay::Vertex * const p2)>;

	private:
		Uint32 SubdivisionDepthCap;
		std::shared_ptr<IDelaunayDAC2DDebug> Debugger;

		void MergeDelaunay(
			DelaunayGraph & leftGraph,
			DelaunayGraph & rightGraph,
			const delaunay::ConvexHull & leftHull,
			const delaunay::ConvexHull & rightHull,
			const delaunay::Tangent & upperTangent,
			const delaunay::Tangent & lowerTangent) const;

		/**
		 * Abstracts out the tangent finding algorithm since finding the top and bottom
		 * tangents used much of the same code.
		 *
		 * @param nextLeftIndex This function retrieves the next face index for the left
		 *                      hull, for finding the top tangent, this means traversing the
		 *                      convex hull in a CCW manner to find the top-right-most
		 *                      tangent vertex.
		 * @param nextRightIndex This function is same as the above, but for the right hull,
		 *                       meaning it should provide the opposite result to the next
		 *                       left index.
		 * @param getWinding This function should get the triangle winding of the 3 provided
		 *                   vertices: the next vertex on either side, the left vertex and
		 *                   the right vertex. This function should return -1 when the next
		 *                   vertex is on the inside of the tangent created by the left and
		 *                   right vertices, 0 for collinear points and 1 when the next
		 *                   vertex is outside of the tangent line.
		 */
		delaunay::Tangent FindRLTangent(
			const delaunay::ConvexHull & leftHull,
			const delaunay::ConvexHull & rightHull) const;

		delaunay::ConvexHull MergeConvexHulls(
			const delaunay::ConvexHull & leftHull,
			const delaunay::ConvexHull & rightHull,
			const delaunay::Tangent & upperTangent,
			const delaunay::Tangent & lowerTangent) const;

		/**
		 * Divides a list of vertices into 2 halfs - a left half and a right half, using the
		 * provided comparator function.
		 */
		void DivideVertexList(
			std::vector<delaunay::Vertex *> & leftHalf,
			std::vector<delaunay::Vertex *> & rightHalf,
			std::vector<delaunay::Vertex *> & vertices,
			const VertexComparator & comparator) const;

		/**
		 * Returns a CW array of vertices representing the convex hull. The division step
		 * alternates between horizontal and vertical divisions. when the subdivision depth is
		 * even, horizontal divisions are used, when odd, vertical divisions are used. This form
		 * of alternating subdivisions avoids the problem of small thin slices that may cause
		 * rounding errors when handling angles.
		 */
		delaunay::ConvexHull Divide(
			DelaunayGraph & results,
			std::vector<delaunay::Vertex *> & vertices,
			const Uint32 subdivisionDepth) const;

	public:
		DelaunayBuilderDAC2D() : SubdivisionDepthCap(0), Debugger(NULL) {}
		DelaunayBuilderDAC2D(
			const Uint32 subdivisionDepthCap,
			const std::shared_ptr<IDelaunayDAC2DDebug> debugger
		) : SubdivisionDepthCap(subdivisionDepthCap), Debugger(debugger)
		{}

		void BuildDelaunayGraph(
			DelaunayGraph & graph,
			const InputPointList & inputPoints) const;

		void MergeDelaunayTileEdge(
			DelaunayGraph & leftGraph, DelaunayGraph & rightGraph,
			const Uint32 lowerTangentLeft, const Uint32 lowerTangentRight,
			const Uint32 upperTangentLeft, const Uint32 upperTangentRight) const;

		/**
		 * @param graphs An array containing the 4 corner Delaunay graphs in the following
		 *               order: top-left, top-right, bottom-left, bottom-right. The second
		 *               value in the pair contains the index of the point in the convex hull
		 *               that is used in the merging procedure.
		 */
		void MergeDelaunayTileCorner(GraphHullIndexArray & graphs) const;
	};
	
	class IDelaunayDAC2DDebug {
	public:
		virtual void MergeStep(
			const DelaunayGraph & leftGraph,
			const DelaunayGraph & rightGraph,
			const delaunay::ConvexHull & leftHull,
			const delaunay::ConvexHull & rightHull,
			const delaunay::Tangent & upperTangent,
			const delaunay::Tangent & lowerTangent,
			const DelaunayBuilderDAC2D::AddedFaceList & leftAddedFaces,
			const DelaunayBuilderDAC2D::AddedFaceList & rightAddedFaces) = 0;

		virtual void StartMergeStep(
			const DelaunayGraph & graph, const Uint32 subdivisionDepth) = 0;
	};
}

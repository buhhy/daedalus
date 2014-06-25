#pragma once

#include <gtest/gtest.h>
#include <Utilities/Graph/Delaunay.h>
#include <Utilities/DataStructures.h>
#include <vector>

TEST(DelaunayDatastructureTest, HandlesTriangleInsertion) {
	using namespace utils::delaunay;
	using namespace utils;
	DelaunayGraph graph({ 0, 0 });
	std::vector<Vector2<>> points;
	points.push_back({ 3, 3 });
	points.push_back({ 4, 5 });
	points.push_back({ 5, 3 });
	points.push_back({ 4, 1 });
	points.push_back({ 2, 1 });
	points.push_back({ 1, 3 });

	for (uint8_t i = 0; i < points.size(); i++)
		graph.AddVertex(points[i], i);

	ASSERT_EQ(graph.VertexCount(), points.size());
}

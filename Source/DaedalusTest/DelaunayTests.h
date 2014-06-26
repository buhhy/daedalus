#pragma once

#include <gtest/gtest.h>
#include <Utilities/Graph/Delaunay.h>
#include <Utilities/DataStructures.h>
#include <Utilities/Hash.h>

#include <iostream>
#include <vector>
#include <random>

using namespace utils::delaunay;
using namespace utils;

class HexagonGraph : public testing::Test {
protected:
	DelaunayGraph * Graph;
	std::vector<Vector2<>> Points;
	std::vector<Vertex *> Vertices;
	std::vector<Face *> Faces;

	virtual void SetUp() override {
		// Inserts a hexagon, point 0 at the center, points 1-5 forming the ring
		Graph = new DelaunayGraph(DelaunayId(0, 0));

		Points.push_back({ 3, 3 });
		Points.push_back({ 4, 5 });
		Points.push_back({ 5, 3 });
		Points.push_back({ 4, 1 });
		Points.push_back({ 2, 1 });
		Points.push_back({ 1, 3 });

		for (uint8_t i = 0; i < Points.size(); i++)
			Vertices.push_back(Graph->AddVertex(Points[i], i));
	
		for (uint8_t i = 1, j = 2; i < Vertices.size(); i++, j++) {
			if (j >= Vertices.size()) j = 1;
			Faces.push_back(Graph->AddFace(Vertices[i], Vertices[j], Vertices[0]));
		}
	}

	virtual void TearDown() override {
		delete Graph;
	}
};

class DelaunayGridGraph : public testing::Test {
protected:
	DelaunayGraph * Graph;
	std::vector<Vector2<>> Points;
	std::vector<Vertex const *> Vertices;
	std::vector<Face const *> Faces;

	virtual void SetUp() override {
		// Use a Mersenne Twister random number generator to create deterministic random numbers
		uint64_t seed = 5000;
		utils::Vector2<int64_t> graphId(0, 0);
		uint8_t minCellPoints = 1;
		uint8_t maxCellPoints = 1;
		uint32_t gridCellCount = 32;

		Graph = new DelaunayGraph(graphId);

		uint32_t mtSeed = utils::HashFromVector(5000, graphId);
		auto randNumPoints = std::bind(
			std::uniform_int_distribution<int>(minCellPoints, maxCellPoints), std::mt19937(mtSeed));
		auto randPosition = std::bind(
			std::uniform_real_distribution<double>(0.1, 0.9), std::mt19937(mtSeed));

		uint8_t numPoints = 0;
		utils::Vector2<> point;
		utils::Vector2<> offset;

		std::vector<std::pair<utils::Vector2<>, uint64_t> > vertexList;
		
		// Create uniform random point distribution, and insert vertices into aggregate list
		for (auto y = 0u; y < gridCellCount; y++) {
			for (auto x = 0u; x < gridCellCount; x++) {
				numPoints = randNumPoints();
				offset.Reset(x, y);
				for (auto n = numPoints - 1; n >= 0; n--) {
					// Set point X, Y to random point within cell
					point.Reset(randPosition(), randPosition());
					point = (point + offset) / (double) gridCellCount;
					Points.push_back(point);
					vertexList.push_back({ point, vertexList.size() });

					std::cout << "point (" << point.X << ", " << point.Y << ")" << std::endl;
				}
			}
		}

		utils::BuildDelaunay2D(*Graph, vertexList);
		Vertices = Graph->GetVertices();
		Faces = Graph->GetFaces();
	}

	virtual void TearDown() override {
		delete Graph;
	}
};

void TestCWTraversal(Vertex const * pivot, const bool testAdjacency) {
	Face * curFace = pivot->GetIncidentFace();

	for (uint8_t i = 0; i < pivot->FaceCount(); i++) {
		auto nextFace = curFace->GetAdjacentFaceCW(pivot);
		if (testAdjacency) {
			auto curVertex = curFace->GetCCWVertex(pivot);
			auto nextVertex = nextFace->GetCWVertex(pivot);
			ASSERT_EQ(curVertex->VertexId(), nextVertex->VertexId());
		}
		curFace = nextFace;
	}

	ASSERT_EQ(pivot->GetIncidentFace(), curFace);
}

void TestCCWTraversal(Vertex const * pivot, const bool testAdjacency) {
	Face * curFace = pivot->GetIncidentFace();

	for (uint8_t i = 0; i < pivot->FaceCount(); i++) {
		auto nextFace = curFace->GetAdjacentFaceCCW(pivot);
		if (testAdjacency) {
			auto curVertex = curFace->GetCWVertex(pivot);
			auto nextVertex = nextFace->GetCCWVertex(pivot);
			ASSERT_EQ(curVertex->VertexId(), nextVertex->VertexId());
		}
		curFace = nextFace;
	}

	ASSERT_EQ(pivot->GetIncidentFace(), curFace);
}

//TEST_F(HexagonGraph, FindsUniqueFaces) {
//	for (uint8_t i = 1, j = 2; i < Vertices.size(); i++, j++) {
//		if (j >= Vertices.size()) j = 1;
//		Face * found = Graph->FindFace(Vertices[i], Vertices[j]);
//		ASSERT_FALSE(found == NULL);
//		ASSERT_EQ(Faces[i - 1]->FaceId(), found->FaceId());
//	}
//}
//
//TEST_F(HexagonGraph, CanEvaluateWhenVerticesAreSurrounded) {
//	// Only the middle vertex is technically surrounded
//	ASSERT_TRUE(Vertices[0]->IsSurrounded());
//	for (uint8_t i = 1; i < Vertices.size(); i++)
//		ASSERT_FALSE(Vertices[i]->IsSurrounded());
//}

//TEST_F(HexagonGraph, HasCorrectDataValues) {
//	ASSERT_EQ(Points.size(), Vertices.size());
//	ASSERT_EQ(Points.size(), Graph->VertexCount());
//	ASSERT_EQ(Points.size() - 1, Graph->FaceCount());
//
//	ASSERT_EQ(Points.size() - 1, Vertices[0]->FaceCount());
//	for (uint8_t i = 1; i < Points.size(); i++)
//		ASSERT_EQ(2, Vertices[i]->FaceCount());
//}
//
//TEST_F(HexagonGraph, TraversesCWFacesProperly) {
//	TestCWTraversal(Vertices[0], true);
//
//	for (uint8_t i = 1; i < Vertices.size(); i++)
//		TestCWTraversal(Vertices[i], false);
//}
//
//TEST_F(HexagonGraph, TraversesCCWFacesProperly) {
//	TestCCWTraversal(Vertices[0], true);
//	
//	for (uint8_t i = 1; i < Vertices.size(); i++)
//		TestCCWTraversal(Vertices[i], false);
//}
//
//TEST_F(HexagonGraph, DeletesFaces) {
//	for (uint8_t i = 0; i < Faces.size(); i++) {
//		Face * face = Faces[i];
//		for (uint8_t v = 0; v < Vertices.size(); v++) {
//			TestCWTraversal(Vertices[v], false);
//			TestCCWTraversal(Vertices[v], false);
//		}
//
//		Graph->RemoveFace(face);
//	}
//}


TEST_F(DelaunayGridGraph, FacesNeighboursAreCoherent) {
	TestCWTraversal(Vertices[0], false);

	for (uint16_t i = 1; i < Vertices.size(); i++) {
		TestCWTraversal(Vertices[i], false);
		TestCCWTraversal(Vertices[i], false);
	}
}

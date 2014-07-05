#pragma once

#include <gtest/gtest.h>
#include <Utilities/Graph/Delaunay.h>
#include <Utilities/DataStructures.h>
#include <Utilities/Hash.h>
#include <Models/Terrain/BiomeRegionLoader.h>

#include <iostream>
#include <vector>
#include <random>

using std::cout;
using std::endl;
using std::vector;
using namespace utils::delaunay;
using namespace utils;

using DelaunayGraphPtr = std::shared_ptr<DelaunayGraph>;

class HexagonGraph : public testing::Test {
protected:
	DelaunayGraph * Graph;
	vector<Vector2<>> Points;
	vector<Vertex *> Vertices;
	vector<Face *> Faces;

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

typedef std::tuple<utils::Vector2<int64_t>, Uint64, Uint32> DelaunayTestParam;
typedef std::pair<DelaunayTestParam, Uint8> DelaunayMultiTestParam;

class DelaunayGridGraph {
protected:
	DelaunayBuilderDAC2D Builder;

	DelaunayGraphPtr ConstructGraph(
		const utils::Vector2<Int64> graphId, const Uint64 seed, const Uint32 gridCellCount
	) const {
		cout << "Generated Delaunay graph with params: " <<
			graphId << ", " << seed << ", " << gridCellCount << endl;
		DelaunayGraphPtr Graph = DelaunayGraphPtr(new DelaunayGraph(graphId));

		uint8_t minCellPoints = 1;
		uint8_t maxCellPoints = 1;
		
		// Use a Mersenne Twister random number generator to create deterministic random numbers
		Uint32 mtSeed = (Uint32) utils::HashFromVector(seed, graphId);
		auto randNumPoints = std::bind(
			std::uniform_int_distribution<int>(minCellPoints, maxCellPoints), std::mt19937(mtSeed));
		auto randPosition = std::bind(
			std::uniform_real_distribution<double>(0.1, 0.9), std::mt19937(mtSeed));

		uint8_t numPoints = 0;
		utils::Vector2<> point;
		utils::Vector2<> offset;

		vector<std::pair<utils::Vector2<>, Uint64> > vertexList;
		
		// Create uniform random point distribution, and insert vertices into aggregate list
		for (auto y = 0u; y < gridCellCount; y++) {
			for (auto x = 0u; x < gridCellCount; x++) {
				numPoints = randNumPoints();
				offset.Reset(x, y);
				for (auto n = numPoints - 1; n >= 0; n--) {
					// Set point X, Y to random point within cell
					point.Reset(randPosition(), randPosition());
					point = (point + offset) / (double) gridCellCount;
					vertexList.push_back({ point, vertexList.size() });

					//std::cout << "point (" << point.X << ", " << point.Y << ")" << std::endl;
				}
			}
		}

		Builder.BuildDelaunayGraph(*Graph, vertexList);
		return Graph;
	}
};

class DelaunayGridGraphSingleTest : public DelaunayGridGraph, public testing::TestWithParam<DelaunayTestParam> {
protected:
	DelaunayGraphPtr GenGraph;

	virtual void SetUp() override {
		const auto & param = GetParam();
		
		GenGraph = ConstructGraph(std::get<0>(param), std::get<1>(param), std::get<2>(param));
	}
};

class DelaunayGridGraphMultiTest : public DelaunayGridGraph, public testing::TestWithParam<DelaunayMultiTestParam> {
protected:
	vector<DelaunayGraphPtr> GenGraphs;

	virtual void SetUp() override {
		const auto & param = GetParam();
		const auto & dparam = param.first;
		const auto & rootPos = std::get<0>(dparam);
		const auto & range = std::get<1>(param);
		
		const Uint16 rowCount = range * 2 + 1;
		DelaunayTestParam * tests = new DelaunayTestParam[rowCount * rowCount];
		for (Int16 y = 0; y < rowCount; y++) {
			for (Int16 x = 0; x < rowCount; x++) {
				GenGraphs.push_back(
					ConstructGraph(
						{ x - range + rootPos.X, y - range + rootPos.Y },
						std::get<1>(dparam), std::get<2>(dparam)));
			}
		}
	}
};

void TestCWTraversal(Vertex const * pivot, const bool testAdjacency) {
	Face * curFace = pivot->GetFirstIncidentFace();

	for (uint8_t i = 0; i < pivot->FaceCount(); i++) {
		auto nextFace = curFace->GetAdjacentFaceCW(pivot);
		if (testAdjacency) {
			auto curVertex = curFace->GetCCWVertex(pivot);
			auto nextVertex = nextFace->GetCWVertex(pivot);
			ASSERT_EQ(curVertex->VertexId(), nextVertex->VertexId());
		}
		curFace = nextFace;
	}

	ASSERT_EQ(pivot->GetFirstIncidentFace(), curFace);
}

void TestCCWTraversal(Vertex const * pivot, const bool testAdjacency) {
	Face * curFace = pivot->GetFirstIncidentFace();

	for (uint8_t i = 0; i < pivot->FaceCount(); i++) {
		auto nextFace = curFace->GetAdjacentFaceCCW(pivot);
		if (testAdjacency) {
			auto curVertex = curFace->GetCWVertex(pivot);
			auto nextVertex = nextFace->GetCCWVertex(pivot);
			ASSERT_EQ(curVertex->VertexId(), nextVertex->VertexId());
		}
		curFace = nextFace;
	}

	ASSERT_EQ(pivot->GetFirstIncidentFace(), curFace);
}

TEST_F(HexagonGraph, FindsUniqueFaces) {
	for (uint8_t i = 1, j = 2; i < Vertices.size(); i++, j++) {
		if (j >= Vertices.size()) j = 1;
		Face * found = Graph->FindFace(Vertices[i], Vertices[j]);
		ASSERT_FALSE(found == NULL);
		ASSERT_EQ(Faces[i - 1]->FaceId(), found->FaceId());
	}
}

TEST_F(HexagonGraph, CanEvaluateWhenVerticesAreSurrounded) {
	// Only the middle vertex is technically surrounded
	ASSERT_TRUE(Vertices[0]->IsSurrounded());
	for (uint8_t i = 1; i < Vertices.size(); i++)
		ASSERT_FALSE(Vertices[i]->IsSurrounded());
}

TEST_F(HexagonGraph, HasCorrectDataValues) {
	ASSERT_EQ(Points.size(), Vertices.size());
	ASSERT_EQ(Points.size(), Graph->VertexCount());
	ASSERT_EQ(Points.size() - 1, Graph->FaceCount());

	ASSERT_EQ(Points.size() - 1, Vertices[0]->FaceCount());
	for (uint8_t i = 1; i < Points.size(); i++)
		ASSERT_EQ(2, Vertices[i]->FaceCount());
}

TEST_F(HexagonGraph, TraversesCWFacesProperly) {
	TestCWTraversal(Vertices[0], true);

	for (uint8_t i = 1; i < Vertices.size(); i++)
		TestCWTraversal(Vertices[i], false);
}

TEST_F(HexagonGraph, TraversesCCWFacesProperly) {
	TestCCWTraversal(Vertices[0], true);
	
	for (uint8_t i = 1; i < Vertices.size(); i++)
		TestCCWTraversal(Vertices[i], false);
}

TEST_F(HexagonGraph, DeletesFaces) {
	for (uint8_t i = 0; i < Faces.size(); i++) {
		Face * face = Faces[i];
		for (uint8_t v = 0; v < Vertices.size(); v++) {
			TestCWTraversal(Vertices[v], false);
			TestCCWTraversal(Vertices[v], false);
		}

		Graph->RemoveFace(face);
	}
}


/**
 * Delaunay triangulation algorithm tests
 */

TEST_P(DelaunayGridGraphSingleTest, GeneratesValidTriangulation) {
	auto vertices = GenGraph->GetVertices();
	for (uint16_t i = 0; i < vertices.size(); i++) {
		TestCWTraversal(vertices[i], false);
		TestCCWTraversal(vertices[i], false);
	}
}

TEST_P(DelaunayGridGraphMultiTest, GeneratesValidTriangulation) {
	for (auto & genGraph : GenGraphs) {
		auto vertices = genGraph->GetVertices();
		for (uint16_t i = 0; i < vertices.size(); i++) {
			TestCWTraversal(vertices[i], false);
			TestCCWTraversal(vertices[i], false);
		}
	}
}

const DelaunayTestParam SingleTests[] = {
	DelaunayTestParam({4, 24}, 12345678, 16),
	DelaunayTestParam({4, 4}, 12345678, 16),
	DelaunayTestParam({3, 4}, 12345678, 16),
	DelaunayTestParam({-25, 3}, 12345678, 16),
	DelaunayTestParam({-24, 3}, 12345678, 16),
	DelaunayTestParam({-28, 3}, 12345678, 16),
	DelaunayTestParam({-79, 6}, 12345678, 16),
	DelaunayTestParam({-5, 5}, 12345678, 64)
};

// THIS WILL CAUSE DEATH IF THE RANGE IS TOO HIGH!
const DelaunayMultiTestParam MultiTests[] = {
	DelaunayMultiTestParam(DelaunayTestParam({-79, 6}, 12345678, 16), 5)
};


// Test delaunay point set
//void Test(DelaunayGraph & graph) {
//	vector<Vector2<> > testPoints;
//	vector<Vertex *> testVertices;
//	testPoints.push_back({ 0.1, 0.2 });
//	testPoints.push_back({ 0.2, 0.1 });
//	testPoints.push_back({ 0.2, 0.3 });
//	testPoints.push_back({ 0.2, 0.4 });
//	testPoints.push_back({ 0.3, 0.2 });
//	testPoints.push_back({ 0.4, 0.4 });
//	testPoints.push_back({ 0.5, 0.3 });
//	testPoints.push_back({ 0.6, 0.4 });
//	testPoints.push_back({ 0.6, 0.2 });
//	testPoints.push_back({ 0.6, 0.1 });

//	
//	/*testPoints.push_back({ 0.1, 0.1 });
//	testPoints.push_back({ 0.1, 0.2 });
//	testPoints.push_back({ 0.1, 0.3 });
//	testPoints.push_back({ 0.2, 0.1 });
//	testPoints.push_back({ 0.3, 0.1 });*/

//	for (auto i = 0u; i < testPoints.size(); i++)
//		testVertices.push_back(graph.AddVertex(testPoints[i], i));

//	graph.ConvexHull = Divide(graph, testVertices, 0);
//}

INSTANTIATE_TEST_CASE_P(DistributedPoints, DelaunayGridGraphSingleTest, testing::ValuesIn(SingleTests));
INSTANTIATE_TEST_CASE_P(DeathTest, DelaunayGridGraphMultiTest, testing::ValuesIn(MultiTests));

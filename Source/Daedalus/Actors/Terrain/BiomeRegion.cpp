#include "Daedalus.h"
#include "BiomeRegion.h"
#include "DebugMeshHelpers.h"
#include "MarchingCubes.h"

ABiomeRegion::ABiomeRegion(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP), BiomeGridScale(0.0) {

	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));
	static ConstructorHelpers::FObjectFinder<UMaterial> wut(TEXT("Material'/Game/TestMaterial.TestMaterial'"));
	//auto material = UMaterialInstanceDynamic::Create((UMaterial *) wut.Object, this);
	//material->SetVectorParameterValue(FName(TEXT("TestProperty")), FLinearColor(1.0, 0.0, 0.0, 1.0));
	//Mesh->SetMaterial(0, material);
	RootComponent = Mesh;
}

void ABiomeRegion::InitializeBiomeRegion(const float scale) {
	BiomeGridScale = scale;
}

void ABiomeRegion::SetBiomeRegionData(const TSharedPtr<terrain::BiomeRegionData> & BiomeRegionData) {
	RegionData = NULL;
	RegionData = BiomeRegionData;
	GenerateBiomeRegionMesh();
}

void ABiomeRegion::GenerateBiomeRegionMesh() {
	double scale = BiomeGridScale;

	uint32 w = RegionData->BiomeGridSize.X;
	uint32 d = RegionData->BiomeGridSize.Y;

	TArray<FMeshTriangle> triangles;
	FVector multiplyVector(scale, scale, scale);
	FVector displacementVector;
	FMeshTriangle tempTri;
	displacementVector.Set(RegionData->BiomeOffset.X, RegionData->BiomeOffset.Y, 0);
	
	// Draw grid lines
	std::vector<utils::Triangle> gridTries;
	utils::Vector3<> tempVector31;
	utils::Vector3<> tempVector32;
	// Y lines
	for (auto y = 0u; y < d; y++) {
		double lineY = (scale * y) / d;
		tempVector31.Reset(0, lineY, 0);
		tempVector32.Reset(scale, lineY, 0);
		utils::CreateLine(gridTries, tempVector31, tempVector32, 5);
	}
	// X lines
	for (auto x = 0u; x < w; x++) {
		double lineX = (scale * x) / d;
		tempVector31.Reset(lineX, 0, 0);
		tempVector32.Reset(lineX, scale, 0);
		utils::CreateLine(gridTries, tempVector31, tempVector32, 5);
	}
	for (auto it : gridTries)
		triangles.Add(FMeshTriangle(it));

	auto & graph = RegionData->DelaunayGraph;

	// Draw points at every Delaunay point
	std::vector<utils::Triangle> pointTries;
	for (auto v : graph.Vertices) {
		tempVector31.Reset(v->Point * scale, 0);
		utils::CreatePoint(pointTries, tempVector31, 10);
	}
	for (auto it : pointTries)
		triangles.Add(FMeshTriangle(it));

	for (auto f : graph.Faces) {
		// Loop through each edge in face, the face will always be convex in both
		// Delaunay and Voronoi graphs, hence triangulation of the face will always
		// be trivial.
		auto * edge = f->Edge;
		uint16 count = 0;
		// Count the number of edges in the face
		for (; edge != f->Edge; edge = edge->NextRightEdge, count++);

		tempTri.Vertex0 =
			(edge->StartVertex->Point.ToFVector() + displacementVector) * BiomeGridScale;
		tempTri.Vertex1 =
			(edge->EndVertex->Point.ToFVector() + displacementVector) * BiomeGridScale;
				
		for (; count > 0; count --) {
			edge = edge->NextRightEdge;
			tempTri.Vertex2 =
				(edge->EndVertex->Point.ToFVector() + displacementVector) * BiomeGridScale;
			triangles.Add(tempTri);
			tempTri.Vertex1 = tempTri.Vertex2;
		}
	}

	Mesh->SetGeneratedMeshTriangles(triangles);
}

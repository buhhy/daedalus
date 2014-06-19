#include "Daedalus.h"
#include "BiomeRegion.h"
#include "DebugMeshHelpers.h"
#include "MarchingCubes.h"

/**
 * Test function to build colored materials from a base material.
 * TODO: write a hex function
 */
inline UMaterialInstanceDynamic * ColoredMaterial(
	UMaterial * base,
	UObject * ref,
	const FLinearColor & color
) {
	auto mat = UMaterialInstanceDynamic::Create((UMaterial *) base, ref);
	mat->SetVectorParameterValue(FName(TEXT("TestProperty")), color);
	return mat;
}



ABiomeRegion::ABiomeRegion(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP), BiomeGridScale(0.0) {

	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));
	TestMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("Material'/Game/TestMaterial.TestMaterial'")).Object;
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
	// Get colors
	auto gridColor = ColoredMaterial(TestMaterial, this, FLinearColor(0.3, 0.3, 0.3));
	auto vertexColor = ColoredMaterial(TestMaterial, this, FLinearColor(0.1, 0.55, 0.1));
	auto edgeColor = ColoredMaterial(TestMaterial, this, FLinearColor(0.55, 0.1, 0.1));
	auto faceColor = ColoredMaterial(TestMaterial, this, FLinearColor(0.1, 0.1, 0.55));


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
	utils::Vector3<> tempVector31, tempVector32, tempVector33;
	// Y lines
	for (auto y = 0u; y < d; y++) {
		double lineY = (scale * y) / d;
		tempVector31.Reset(0, lineY, 0);
		tempVector32.Reset(scale, lineY, 0);
		utils::CreateLine(gridTries, tempVector31, tempVector32, 0.2);
	}
	// X lines
	for (auto x = 0u; x < w; x++) {
		double lineX = (scale * x) / d;
		tempVector31.Reset(lineX, 0, 0);
		tempVector32.Reset(lineX, scale, 0);
		utils::CreateLine(gridTries, tempVector31, tempVector32, 0.2);
	}
	for (auto it : gridTries)
		triangles.Add(FMeshTriangle(it, gridColor));

	auto & graph = RegionData->DelaunayGraph;

	// Draw points at every Delaunay point
	std::vector<utils::Triangle> pointTries;
	auto verts = graph.GetVertices();
	for (auto v : verts) {
		tempVector31.Reset(v->Point * scale, 0);
		utils::CreatePoint(pointTries, tempVector31, 0.8);
	}
	for (auto it : pointTries) triangles.Add(FMeshTriangle(it, vertexColor));

	//// Draw convex hull
	//std::vector<utils::Triangle> edgeTries;
	//auto hull = graph.ConvexHull;
	//for (uint64 i = 0, j = 1; i < hull.size(); i++, j++) {
	//	if (j == hull.size())
	//		j = 0;

	//	tempVector31.Reset(hull[i]->Point * scale, 0);
	//	tempVector32.Reset(hull[j]->Point * scale, 0);
	//	utils::CreateLine(edgeTries, tempVector31, tempVector32, 3);
	//}
	//for (auto it : edgeTries) triangles.Add(FMeshTriangle(it, edgeColor));

	// Draw lines at every Delaunay edge
	std::vector<utils::Triangle> edgeTries;
	auto edges = graph.GetUniqueEdges();
	for (auto e : edges) {
		tempVector31.Reset(e.Start->Point * scale, 0);
		tempVector32.Reset(e.End->Point * scale, 0);
		utils::CreateLine(edgeTries, tempVector31, tempVector32, 0.25);
	}
	for (auto it : edgeTries) triangles.Add(FMeshTriangle(it, edgeColor));

	// Draw each Delaunay triangle
	std::vector<utils::Triangle> faceTries;
	auto faces = graph.GetFaces();
	for (auto f : faces) {
		if (!f->IsDegenerate()) {
			tempVector31.Reset(f->Vertices[0]->Point * scale, 0);
			tempVector32.Reset(f->Vertices[1]->Point * scale, 0);
			tempVector33.Reset(f->Vertices[2]->Point * scale, 0);
			faceTries.push_back(utils::Triangle(tempVector31, tempVector32, tempVector33));
		}
	}
	for (auto it : faceTries) triangles.Add(FMeshTriangle(it, faceColor));

	Mesh->SetGeneratedMeshTriangles(triangles);
}

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

void ABiomeRegion::SetBiomeRegionData(
	const std::shared_ptr<terrain::BiomeRegionData> & BiomeRegionData
) {
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

	float gridLineWidth = 0.25;
	float pointSize = 0.9;
	float edgeWidth = 0.6;
	float heightMultiplier = 250;
	double scale = BiomeGridScale;

	uint32 size = RegionData->GetBiomeGridSize();

	TArray<FMeshTriangle> triangles;
	FVector multiplyVector(scale, scale, scale);
	FVector displacementVector;
	displacementVector.Set(
		RegionData->GetBiomeRegionOffset().X, RegionData->GetBiomeRegionOffset().Y, 0);
	
	// Draw grid lines
	std::vector<utils::Triangle> gridTries;
	utils::Vector3<> tempVector31, tempVector32, tempVector33;
	// Y lines
	for (auto y = 0u; y < size; y++) {
		double lineY = (scale * y) / size;
		tempVector31.Reset(0, lineY, 0);
		tempVector32.Reset(scale, lineY, 0);
		utils::CreateLine(gridTries, tempVector31, tempVector32, gridLineWidth);
	}
	// X lines
	for (auto x = 0u; x < size; x++) {
		double lineX = (scale * x) / size;
		tempVector31.Reset(lineX, 0, 0);
		tempVector32.Reset(lineX, scale, 0);
		utils::CreateLine(gridTries, tempVector31, tempVector32, gridLineWidth);
	}
	for (auto it : gridTries)
		triangles.Add(FMeshTriangle(it, gridColor));

	auto & graph = RegionData->DelaunayGraph;

	// Draw points at every Delaunay point
	std::vector<utils::Triangle> pointTries;
	auto verts = graph.GetVertices();
	for (auto v : verts) {
		if (v->IsForeign())
			continue;
		auto biome = RegionData->GetBiomeAt(v->VertexId());
		tempVector31.Reset(v->GetPoint() * scale, biome->GetElevation() * heightMultiplier);
		utils::CreatePoint(pointTries, tempVector31, pointSize);
	}
	for (auto it : pointTries) triangles.Add(FMeshTriangle(it, vertexColor));

	//// Draw convex hull
	//std::vector<utils::Triangle> edgeTries;
	//auto hull = graph.ConvexHull;
	//for (uint64 i = 0, j = 1; i < hull.size(); i++, j++) {
	//	if (j == hull.size())
	//		j = 0;

	//	tempVector31.Reset(hull[i]->GetPoint() * scale, 0);
	//	tempVector32.Reset(hull[j]->GetPoint() * scale, 0);
	//	utils::CreateLine(edgeTries, tempVector31, tempVector32, 3);
	//}
	//for (auto it : edgeTries) triangles.Add(FMeshTriangle(it, edgeColor));

	// Draw lines at every Delaunay edge
	std::vector<utils::Triangle> edgeTries;
	auto edges = graph.GetUniqueEdges();
	for (auto e : edges) {
		if (e.Start->IsForeign() || e.End->IsForeign())
			continue;
		auto biome1 = RegionData->GetBiomeAt(e.Start->VertexId());
		auto biome2 = RegionData->GetBiomeAt(e.End->VertexId());
		tempVector31.Reset(e.Start->GetPoint() * scale, biome1->GetElevation() * heightMultiplier);
		tempVector32.Reset(e.End->GetPoint() * scale, biome2->GetElevation() * heightMultiplier);
		utils::CreateLine(edgeTries, tempVector31, tempVector32, edgeWidth);
	}
	for (auto it : edgeTries) triangles.Add(FMeshTriangle(it, edgeColor));

	// Draw each Delaunay triangle
	std::vector<utils::Triangle> faceTries;
	auto faces = graph.GetFaces();
	for (auto f : faces) {
		if (!f->IsDegenerate()) {
			if (f->Vertices[0]->IsForeign() || f->Vertices[1]->IsForeign() || f->Vertices[2]->IsForeign())
				continue;
			auto biome1 = RegionData->GetBiomeAt(f->Vertices[0]->VertexId());
			auto biome2 = RegionData->GetBiomeAt(f->Vertices[1]->VertexId());
			auto biome3 = RegionData->GetBiomeAt(f->Vertices[2]->VertexId());
			tempVector31.Reset(f->Vertices[0]->GetPoint() * scale, biome1->GetElevation() * heightMultiplier);
			tempVector32.Reset(f->Vertices[1]->GetPoint() * scale, biome2->GetElevation() * heightMultiplier);
			tempVector33.Reset(f->Vertices[2]->GetPoint() * scale, biome3->GetElevation() * heightMultiplier);
			faceTries.push_back(utils::Triangle(tempVector31, tempVector32, tempVector33));
		}
	}
	for (auto it : faceTries) triangles.Add(FMeshTriangle(it, faceColor));

	Mesh->SetGeneratedMeshTriangles(triangles);
}

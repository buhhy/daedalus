#include "Daedalus.h"
#include "BiomeRegion.h"
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

void ABiomeRegion::InitializeBiomeRegion(const double scale) {
	BiomeGridScale = scale;
}

void ABiomeRegion::SetBiomeRegionData(const TSharedPtr<terrain::BiomeRegionData> & BiomeRegionData) {
	RegionData = NULL;
	RegionData = BiomeRegionData;
	GenerateBiomeRegionMesh();
}

void ABiomeRegion::GenerateBiomeRegionMesh() {
	uint32 w = RegionData->BiomeGridSize.X;
	uint32 d = RegionData->BiomeGridSize.Y;

	TArray<FMeshTriangle> triangles;
	TArray<utils::Triangle> tempTris;
	FVector multiplyVector(BiomeGridScale, BiomeGridScale, BiomeGridScale);
	FVector displacementVector;

	utils::GridCell gridCell;
	auto & graph = RegionData->DelaunayGraph;
	auto & points = RegionData->PointSet;

	for (uint32 x = 0; x < w; x++) {
		for (uint32 y = 0; y < d; y++) {
		}
	}

	Mesh->SetGeneratedMeshTriangles(triangles);
}

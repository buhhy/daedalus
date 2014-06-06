#include "Daedalus.h"
#include "Chunk.h"
#include "MarchingCubes.h"

AChunk::AChunk(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP) {

	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));
	static ConstructorHelpers::FObjectFinder<UMaterial> wut(TEXT("Material'/Game/TestMaterial.TestMaterial'"));
	//auto material = UMaterialInstanceDynamic::Create((UMaterial *) wut.Object, this);
	//material->SetVectorParameterValue(FName(TEXT("TestProperty")), FLinearColor(1.0, 0.0, 0.0, 1.0));
	//Mesh->SetMaterial(0, material);
	RootComponent = Mesh;
}

void AChunk::InitializeChunk(const terrain::TerrainGeneratorParameters & params) {
	TerrainGenParams = params;
}

void AChunk::SetChunkData(const TSharedPtr<terrain::ChunkData> & chunkData) {
	ChunkData = NULL;
	ChunkData = chunkData;
	GenerateChunkMesh();
}

void AChunk::GenerateChunkMesh() {
	uint32 w = TerrainGenParams.ChunkPolygonSize.X;
	uint32 h = TerrainGenParams.ChunkPolygonSize.Y;
	uint32 d = TerrainGenParams.ChunkPolygonSize.Z;

	float unitSize = TerrainGenParams.ChunkScale;

	TArray<FMeshTriangle> triangles;
	TArray<utils::Triangle> tempTris;
	FVector multiplyVector(unitSize, unitSize, unitSize);
	FVector displacementVector;

	utils::GridCell gridCell;
	auto & density = ChunkData->DensityData;

	for (uint32 x = 0; x < w; x++) {
		for (uint32 y = 0; y < d; y++) {
			for (uint32 z = 0; z < h; z++) {
				gridCell.Initialize(
					density.Get(x, y, z),
					density.Get(x, y, z + 1),
					density.Get(x, y + 1, z),
					density.Get(x, y + 1, z + 1),
					density.Get(x + 1, y, z),
					density.Get(x + 1, y, z + 1),
					density.Get(x + 1, y + 1, z),
					density.Get(x + 1, y + 1, z + 1));

				tempTris.Reset();
				utils::MarchingCube(tempTris, 0.5, gridCell);

				displacementVector.Set(x, y, z);

				for (auto it = tempTris.CreateConstIterator(); it; ++it) {
					FMeshTriangle tri;
					tri.Vertex0 = (it->points[0] + displacementVector) * multiplyVector;
					tri.Vertex1 = (it->points[1] + displacementVector) * multiplyVector;
					tri.Vertex2 = (it->points[2] + displacementVector) * multiplyVector;
					triangles.Add(tri);
				}
			}
		}
	}

	Mesh->SetGeneratedMeshTriangles(triangles);
}

#include "Daedalus.h"
#include "Chunk.h"
#include "MarchingCubes.h"

AChunk::AChunk(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP) {

	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));
	TestMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("Material'/Game/TestMaterial.TestMaterial'")).Object;
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
	auto material = UMaterialInstanceDynamic::Create((UMaterial *) TestMaterial, this);

	uint32 size = TerrainGenParams.GridCellCount;

	double scale = TerrainGenParams.ChunkScale / size;

	std::vector<utils::Triangle> tempTris;
	utils::Vector3<> displacementVector;

	utils::GridCell gridCell;
	auto & density = ChunkData->DensityData;
	std::vector<utils::Triangle> triangles;

	for (uint32 x = 0; x < size; x++) {
		for (uint32 y = 0; y < size; y++) {
			for (uint32 z = 0; z < size; z++) {
				gridCell.Initialize(
					density.Get(x, y, z),
					density.Get(x, y, z + 1),
					density.Get(x, y + 1, z),
					density.Get(x, y + 1, z + 1),
					density.Get(x + 1, y, z),
					density.Get(x + 1, y, z + 1),
					density.Get(x + 1, y + 1, z),
					density.Get(x + 1, y + 1, z + 1));

				tempTris.clear();
				utils::MarchingCube(tempTris, 0.5, gridCell);

				displacementVector.Reset(x, y, z);

				for (auto & trit : tempTris) {
					utils::Triangle tri(
						(trit.Point1 + displacementVector) * scale,
						(trit.Point2 + displacementVector) * scale,
						(trit.Point3 + displacementVector) * scale);
					triangles.push_back(tri);
				}
			}
		}
	}

	if (triangles.size() > 0) {
		TArray<FMeshTriangle> meshTriangles;
		for (auto it : triangles)
			meshTriangles.Add(FMeshTriangle(it, material));
		Mesh->SetGeneratedMeshTriangles(meshTriangles);
	}
}

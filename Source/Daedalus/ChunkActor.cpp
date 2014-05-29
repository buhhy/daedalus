#include "Daedalus.h"
#include "ChunkActor.h"
#include "MarchingCubes.h"

AChunkActor::AChunkActor(const class FPostConstructInitializeProperties & PCIP)
: Super(PCIP) {

	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));

	uint32 Size = FMath::Pow(2, 6) + 1;
	uint32 Seed = 123456;

	InitializeChunk(
		utils::Vector3<uint64>(Size, Size, Size),
		utils::Vector3<int64>(0, 0, 0),
		Seed);
	TestRender();
	RootComponent = Mesh;
}

void AChunkActor::InitializeChunk(
	const utils::Vector3<uint64> & chunkSize,
	const utils::Vector3<int64> & chunkOffset,
	uint64 seed
) {
	ChunkData.InitializeChunk(chunkSize, chunkOffset, seed);
}

void AChunkActor::TestRender() {
	auto size = ChunkData.Size();
	uint32 w = size.X;
	uint32 h = size.Y;
	uint32 d = size.Z;

	float unitSize = 40.0;

	TArray<FGeneratedMeshTriangle> triangles;
	TArray<utils::Triangle> tempTris;
	FVector multiplyVector(unitSize, unitSize, unitSize);
	FVector displacementVector;

	utils::GridCell gridCell;

	for (uint32 x = 0; x < w - 1; x++) {
		for (uint32 y = 0; y < d - 1; y++) {
			for (uint32 z = 0; z < h - 1; z++) {
				auto density = ChunkData.Density();

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
					FGeneratedMeshTriangle tri;
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

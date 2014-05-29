#include "Daedalus.h"
#include "Chunk.h"
#include "MarchingCubes.h"

AChunk::AChunk(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP) {

	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));

	uint32 Size = FMath::Pow(2, 6) + 1;
	uint32 Seed = 123456;

	Densities.Init(Size, Size, Size);
	InitializeChunk(Size, Size, Size, 0, 0, 0, Seed);
	/*for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				Densities.SetDensityAt(3 + i, 3 + j, 3 + k, 1);*/
	RunDiamondSquare();
	TestRender();
	RootComponent = Mesh;
}

double AChunk::GenerateRandomNumber(
	uint16 relativeX,
	uint16 relativeY,
	uint16 relativeZ
) {
	uint64 collapsed = ((relativeX + OffsetX) * 6151 +
		(relativeY + OffsetY) * 3079 +
		(relativeZ + OffsetZ) * 1543 + 769) ^ Seed * 389;

	// FNV-1a hashing
	uint64 hash = 14695981039346656037;
	for (auto i = 0; i < 8; i++) {
		hash *= 1099511628211;
		hash ^= collapsed & 0xff;
		collapsed >>= 8;
	}

	return (double)hash / (double)MAX_uint64;
}

void AChunk::InitializeChunk(
	uint16 chunkWidth, uint16 chunkDepth, uint16 chunkHeight,
	uint64 offsetX, uint64 offsetY, uint64 offsetZ,
	uint64 seed
) {
	OffsetX = offsetX;
	OffsetY = offsetY;
	OffsetZ = offsetZ;
	Seed = seed;
}

void AChunk::SetDefaultHeight(uint32 height) {
	for (uint32 x = 0; x < Densities.Width; x++) {
		for (uint32 y = 0; y < Densities.Depth; y++) {
			//for (uint32 z = 0; z < Densities.Height && z <= height; z++)
				Densities.SetDensityAt(x, y, height, 1);
		}
	}
}

void AChunk::RunDiamondSquare() {
	uint32 w = Densities.Width;
	uint32 h = Densities.Height;
	uint32 d = Densities.Depth;
	float ** heightMap = new float *[w];

	// Initialization
	for (uint32 x = 0; x < w; x++) {
		heightMap[x] = new float[d];

		for (uint32 z = 0; z < d; z++)
			heightMap[x][z] = 0.0;
	}

	// Diamond square algorithm
	float perturb = 1.0;
	for (uint32 interval = w; interval > 1; interval /= 2) {
		uint32 halfInterval = interval / 2;

		// Diamond step
		for (uint32 x = halfInterval; x < w; x += interval) {
			for (uint32 y = halfInterval; y < d; y += interval) {
				auto topl = heightMap[x - halfInterval][y - halfInterval];
				auto topr = heightMap[x + halfInterval][y - halfInterval];
				auto botl = heightMap[x - halfInterval][y + halfInterval];
				auto botr = heightMap[x + halfInterval][y + halfInterval];
				auto avg = (topl + topr + botl + botr) / 4;
				auto rand = (GenerateRandomNumber(x, y, FMath::Round(avg)) * 2.0 - 1.0) * perturb;
				heightMap[x][y] = avg + rand;
			}
		}

		// Square step
		for (uint32 x = 0, i = 0; x < w; x += interval / 2, i++) {
			for (uint32 y = 0, j = 0; y < d; y += interval / 2, j++) {
				if ((i % 2 != 0) != (j % 2 != 0)) {
					auto top = y == 0 ? 0.0 : heightMap[x][y - halfInterval];
					auto bottom = y == d - 1 ? 0.0 : heightMap[x][y + halfInterval];
					auto left = x == 0 ? 0.0 : heightMap[x - halfInterval][y];
					auto right = x == w - 1 ? 0.0 : heightMap[x + halfInterval][y];
					auto avg = (top + bottom + left + right) / 4;
					auto rand = (GenerateRandomNumber(x, y, FMath::Round(avg)) * 2.0 - 1.0) * perturb;
					heightMap[x][y] = avg + rand;
				}
			}
		}

		perturb /= 2;
	}

	for (uint32 x = 0; x < w; x++) {
		for (uint32 y = 0; y < d; y++) {
			uint32 height = FMath::Round(heightMap[x][y] * 5.5 + h / 2);
			for (uint32 z = 0; z <= height; z++)
				Densities.SetDensityAt(x, y, z, 1);
		}
	}
}

void AChunk::TestRender() {
	uint32 w = Densities.Width;
	uint32 h = Densities.Height;
	uint32 d = Densities.Depth;

	float unitSize = 40.0;

	TArray<FGeneratedMeshTriangle> triangles;
	TArray<utils::Triangle> tempTris;
	FVector multiplyVector(unitSize, unitSize, unitSize);
	FVector displacementVector;

	utils::GridCell gridCell;

	for (uint32 x = 0; x < w - 1; x++) {
		for (uint32 y = 0; y < d - 1; y++) {
			for (uint32 z = 0; z < h - 1; z++) {
				gridCell.Initialize(
					Densities.GetDensityAt(x, y, z) > 0,
					Densities.GetDensityAt(x, y, z + 1) > 0,
					Densities.GetDensityAt(x, y + 1, z) > 0,
					Densities.GetDensityAt(x, y + 1, z + 1) > 0,
					Densities.GetDensityAt(x + 1, y, z) > 0,
					Densities.GetDensityAt(x + 1, y, z + 1) > 0,
					Densities.GetDensityAt(x + 1, y + 1, z) > 0,
					Densities.GetDensityAt(x + 1, y + 1, z + 1) > 0);

				tempTris.Reset();
				utils::MarchingCube(tempTris, gridCell);

				displacementVector.Set(x, y, z);

				for (auto it = tempTris.CreateConstIterator(); it; ++it) {
					FGeneratedMeshTriangle tri;
					tri.Vertex0 = (it->points[0] + displacementVector) * multiplyVector;
					tri.Vertex1 = (it->points[1] + displacementVector) * multiplyVector;
					tri.Vertex2 = (it->points[2] + displacementVector) * multiplyVector;
					triangles.Add(tri);
				}
			}

			/*uint32 height = 0;
			for (height; height < h; height++) {
				if (Densities.GetDensityAt(x, height, z) == 0)
					break;
			}

			FVector tl(x * unitSize, z * unitSize, height * unitSize);
			FVector tr((x + 1) * unitSize, z * unitSize, height * unitSize);
			FVector bl(x * unitSize, (z + 1) * unitSize, height * unitSize);
			FVector br((x + 1) * unitSize, (z + 1) * unitSize, height * unitSize);

			FGeneratedMeshTriangle tri1;
			tri1.Vertex0 = bl;
			tri1.Vertex1 = tr;
			tri1.Vertex2 = tl;

			FGeneratedMeshTriangle tri2;
			tri2.Vertex0 = bl;
			tri2.Vertex1 = br;
			tri2.Vertex2 = tr;

			triangles.Add(tri1);
			triangles.Add(tri2);*/
		}
	}

	Mesh->SetGeneratedMeshTriangles(triangles);
}

#include "Daedalus.h"
#include "ChunkData.h"
#include "MarchingCubes.h"
#include "Random.h"

ChunkData::ChunkData() {
	uint32 Size = FMath::Pow(2, 6) + 1;
	uint32 Seed = 123456;

	DensityData.Init(Size, Size, Size);
	MaterialData.Init(Size, Size, Size);
	InitializeChunk(
		utils::Vector3<uint64>(Size, Size, Size),
		utils::Vector3<int64>(0, 0, 0), Seed);
	RunDiamondSquare();
}

ChunkData::~ChunkData() { }

void ChunkData::InitializeChunk(
	const utils::Vector3<uint64> & chunkSize,
	const utils::Vector3<int64> & chunkOffset,
	uint64 seed
) {
	this->ChunkSize = chunkSize;
	this->ChunkOffset = chunkOffset;
	this->Seed = seed;
}

void ChunkData::SetDefaultHeight(uint32 height) {
	for (uint32 x = 0; x < DensityData.Width; x++) {
		for (uint32 y = 0; y < DensityData.Depth; y++) {
			//for (uint32 z = 0; z < DensityData.Height && z <= height; z++)
			DensityData.Set(x, y, height, 1);
		}
	}
}

const utils::Vector3<uint64> & ChunkData::Size() const {
	return ChunkSize;
}

const utils::Tensor3<float> & ChunkData::Density() const {
	return DensityData;
}

void ChunkData::RunDiamondSquare() {
	uint32 w = DensityData.Width;
	uint32 h = DensityData.Height;
	uint32 d = DensityData.Depth;
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

				auto rand = (utils::generateRandomNumber(Seed, ChunkSize * ChunkOffset + utils::Vector3<uint64>(x, y, FMath::Round(avg))) * 2.0 - 1.0) * perturb;
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

					auto rand = (utils::generateRandomNumber(Seed, ChunkSize * ChunkOffset + utils::Vector3<uint64>(x, y, FMath::Round(avg))) * 2.0 - 1.0) * perturb;
					heightMap[x][y] = avg + rand;
				}
			}
		}

		perturb /= 2;
	}

	for (uint32 x = 0; x < w; x++) {
		for (uint32 y = 0; y < d; y++) {
			float height = heightMap[x][y] * 5.5 + h / 2;
			height = FMath::Round(height * 2.0) / 2.0; // Round to nearest .5
			for (uint64 z = 0; height > 0; height -= 1, z++)
				DensityData.Set(x, y, z, 1.0);
		}
	}
}

#include <Daedalus.h>
#include "Chunk.h"
#include <Utilities/UnrealBridge.h>
#include <Utilities/Mesh/MarchingCubes.h>
#include <Utilities/Mesh/DebugMeshHelpers.h>

using namespace utils;
using namespace terrain;
using namespace item;

using ChunkDataSet = AChunk::ChunkDataSet;

AChunk::AChunk(const FPostConstructInitializeProperties & PCIP)
	: Super(PCIP), ChunkData(NULL)
{
	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));
	TestMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("Material'/Game/TestMaterial.TestMaterial'")).Object;
	this->RootComponent = Mesh;
}

void AChunk::SpawnItem(const item::ItemDataPtr & itemData) {
	const FRotator defaultRotator(0, 0, 0);
	auto params = FActorSpawnParameters();
	params.Name = *FString::Printf(TEXT("(%lld,%lld,%lld)PlacedItem%llu"),
		itemData->ItemId.second.first.X,
		itemData->ItemId.second.first.Y,
		itemData->ItemId.second.first.Z,
		itemData->ItemId.first);
	UE_LOG(LogTemp, Warning, TEXT("(%lld,%lld,%lld)PlacedItem%llu"),
		itemData->ItemId.second.first.X,
		itemData->ItemId.second.first.Y,
		itemData->ItemId.second.first.Z,
		itemData->ItemId.first);
	auto actor = GetWorld()->SpawnActor<AItem>(
		ItemFactory->GetItemClass(itemData),
		ToFVector(TerrainGenParams.GetChunkInnerPosition(itemData->ItemId.second)),
		defaultRotator,
		params);
	PlacedItems.Add({ itemData->ItemId, actor });
	actor->AttachRootComponentToActor(this);
}

void AChunk::RemoveItem(const item::ItemDataPtr & itemData) {
	Uint32 index = 0;
	for (const auto & it : PlacedItems) {
		if (it.ItemId == itemData->ItemId) {
			it.ItemActor->Destroy();
			PlacedItems.RemoveAt(index);
			break;
		}
		++index;
	}
}

void AChunk::ReceiveDestroyed() {
	for (const auto & it : PlacedItems)
		it.ItemActor->Destroy();
	Super::ReceiveDestroyed();
}

void AChunk::InitializeChunk(
	const TerrainGeneratorParameters & params,
	const UItemFactory * itemFactory
) {
	TerrainGenParams = params;
	ItemFactory = itemFactory;
}

void AChunk::SetChunkData(const ChunkDataSet & chunkData) {
	ChunkData = chunkData;
	GenerateChunkMesh();

	ItemDataPtr test(new ItemData(I_Chest, ItemDataId(0u, ChunkPositionVector(ChunkData.Get(0, 0, 0)->ChunkOffset, { 0, 0, 0 }))));
	SpawnItem(test);
}

void AChunk::GenerateChunkMesh() {
	auto material = UMaterialInstanceDynamic::Create((UMaterial *) TestMaterial, this);

	Uint32 size = TerrainGenParams.GridCellCount;

	double scale = TerrainGenParams.ChunkScale / size;

	std::vector<Triangle3D> tempTris;
	Vector3D<> displacementVector;

	GridCell gridCell;
	Tensor3D<float> densityDataPoints(size + 1);
	
	// Populate the density data
	for (Uint32 x = 0; x < size + 1; x++) {
		for (Uint32 y = 0; y < size + 1; y++) {
			for (Uint32 z = 0; z < size + 1; z++) {
				ChunkDataPtr mainData = ChunkData.Get(x / size, y / size, z / size);
				densityDataPoints.Set(
					x, y, z, mainData->DensityData.Get(x % size, y % size, z % size));
			}
		}
	}

	// Populate the neighbouring data
	std::vector<Triangle3D> triangles;

	for (Uint32 x = 0; x < size; x++) {
		for (Uint32 y = 0; y < size; y++) {
			for (Uint32 z = 0; z < size; z++) {
				gridCell.Initialize(
					densityDataPoints.Get(x, y, z),
					densityDataPoints.Get(x, y, z + 1),
					densityDataPoints.Get(x, y + 1, z),
					densityDataPoints.Get(x, y + 1, z + 1),
					densityDataPoints.Get(x + 1, y, z),
					densityDataPoints.Get(x + 1, y, z + 1),
					densityDataPoints.Get(x + 1, y + 1, z),
					densityDataPoints.Get(x + 1, y + 1, z + 1));

				tempTris.clear();
				MarchingCube(tempTris, 0.5, gridCell);

				displacementVector.Reset(x, y, z);

				for (auto & trit : tempTris) {
					Triangle3D tri(
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

#include <Daedalus.h>
#include "Chunk.h"

#include <Utilities/UnrealBridge.h>
#include <Utilities/Mesh/MarchingCubes.h>
#include <Utilities/Mesh/DebugMeshHelpers.h>

#include <cmath>

using namespace utils;
using namespace terrain;
using namespace items;

using ChunkDataSet = AChunk::ChunkDataSet;

AChunk::AChunk(const FPostConstructInitializeProperties & PCIP)
	: Super(PCIP), ChunkNeighbourData(NULL), ItemIdCounter(0)
{
	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));
	TestMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("Material'/Game/TestMaterial.TestMaterial'")).Object;
	this->RootComponent = Mesh;
}

AItem * AChunk::SpawnItem(const ItemDataPtr & itemData) {
	const FRotator defaultRotator(0, 0, 0);
	auto params = FActorSpawnParameters();
	params.Name = *FString::Printf(TEXT("(%lld,%lld,%lld)PlacedItem%llu"),
		itemData->Position.first.X,
		itemData->Position.first.Y,
		itemData->Position.first.Z,
		itemData->ItemId);
	UE_LOG(LogTemp, Warning, TEXT("(%lld,%lld,%lld)PlacedItem%llu"),
		itemData->Position.first.X,
		itemData->Position.first.Y,
		itemData->Position.first.Z,
		itemData->ItemId);
	auto actor = GetWorld()->SpawnActor<AItem>(
		AItem::StaticClass(),
		ToFVector(TerrainGenParams->ToRealInnerCoordinates(itemData->Position)),
		FRotator(0, 0, 0), params);
	PlacedItems.Add({ itemData->ItemId, actor });
	actor->Initialize(itemData);
	actor->AttachRootComponentToActor(this);
	return actor;
}

ItemDataPtr AChunk::RemoveItem(const ItemDataPtr & itemData) {
	Uint32 index = 0;
	ItemDataPtr removed = NULL;

	for (const auto & it : PlacedItems) {
		if (it.ItemId == itemData->ItemId) {
			it.ItemActor->Destroy();
			PlacedItems.RemoveAt(index);
			removed = it.ItemActor->GetItemData();
			break;
		}
		++index;
	}

	return removed;
}

void AChunk::ReceiveDestroyed() {
	for (const auto & it : PlacedItems)
		it.ItemActor->Destroy();
	Super::ReceiveDestroyed();
}

void AChunk::InitializeChunk(const TerrainGeneratorParameters * params) {
	SolidTerrain.Reset(params->GridCellCount, params->GridCellCount, params->GridCellCount, false);
	TerrainGenParams = params;
}

void AChunk::SetChunkData(const ChunkDataSet & chunkData) {
	ChunkNeighbourData = chunkData;
	CurrentChunkData = chunkData.Get(0, 0, 0);
	// Make sure the item ID counter is at least 1 larger than the current largest ID
	for (const auto & itemData : CurrentChunkData->PlacedItems) {
		if (itemData->ItemId >= ItemIdCounter)
			ItemIdCounter = itemData->ItemId + 1;
	}
	GenerateChunkMesh();
}

AItem * AChunk::CreateItem(const items::ItemDataPtr & itemData, const bool preserveId) {
	if (!preserveId)
		itemData->ItemId = ItemIdCounter++;
	itemData->bIsPlaced = true;
	CurrentChunkData->PlacedItems.push_back(itemData);
	return SpawnItem(itemData);
}

void AChunk::GenerateChunkMesh() {
	auto material = UMaterialInstanceDynamic::Create((UMaterial *) TestMaterial, this);

	Uint32 size = TerrainGenParams->GridCellCount;

	double scale = TerrainGenParams->ChunkScale / size;

	std::vector<Triangle3D> tempTris;
	Vector3D<double> displacementVector;

	GridCell gridCell;
	Tensor3D<float> densityDataPoints(size + 1);
	
	// Populate the density data
	for (Uint32 x = 0; x < size + 1; x++) {
		for (Uint32 y = 0; y < size + 1; y++) {
			for (Uint32 z = 0; z < size + 1; z++) {
				ChunkDataPtr mainData = ChunkNeighbourData.Get(x / size, y / size, z / size);
				densityDataPoints.Set(
					x, y, z, mainData->DensityData.Get(x % size, y % size, z % size));
			}
		}
	}

	std::vector<Triangle3D> triangles;

	// Build the mesh
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

				// Set the solid terrain cache if this grid cell is filled
				if (gridCell.Sum() > FLOAT_ERROR)
					SolidTerrain.Set(x, y, z, true);

				tempTris.clear();
				MarchingCube(tempTris, 0, gridCell);

				displacementVector.Reset((double) x, (double) y, (double) z);

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

bool AChunk::TerrainIntersection(
	Vector3D<Int64> & collisionIndex,
	Vector3D<Int64> & precollisionIndex,
	const Ray3D & ray,
	const double maxDistance
) const {
	FastVoxelTraversalIterator fvt(
		TerrainGenParams->ChunkGridUnitSize, 0, TerrainGenParams->GridCellCount,
		{ ray.Origin * TerrainGenParams->ChunkScale, ray.Direction }, maxDistance);

	const auto & x = fvt.GetCurrentCell();

	bool found = false;
	Vector3D<Int64> zv(0);

	for (Uint16 i = 0; fvt.IsValid(); i++) {
		const auto & current = fvt.GetCurrentCell();
		if (current.IsBoundedBy(0, TerrainGenParams->GridCellCount)) {
			found = SolidTerrain.Get(current.X, current.Y, current.Z);
			if (found)
				break;
		}

		fvt.Next();
	}

	if (found) {
		collisionIndex = fvt.GetCurrentCell();
		precollisionIndex = fvt.GetPreviousCell();
	}

	return found;
}

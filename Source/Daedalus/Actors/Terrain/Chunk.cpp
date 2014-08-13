#include <Daedalus.h>
#include "Chunk.h"

#include <Utilities/UnrealBridge.h>
#include <Utilities/Mesh/MarchingCubes.h>
#include <Utilities/Mesh/DebugMeshHelpers.h>

#include <cmath>

namespace terrain {
	TerrainRaytraceResult::TerrainRaytraceResult(
		const TerrainRaytraceResultType type,
		AActor * const actor,
		const ChunkPositionVector & pos
	) : Type(type), Result(actor), EntryPosition(pos)
	{}

	TerrainRaytraceResult::TerrainRaytraceResult(
		AChunk * const chunk, const ChunkPositionVector & pos
	) : TerrainRaytraceResult(E_Terrain, chunk, pos)
	{}

	TerrainRaytraceResult::TerrainRaytraceResult(
		AItem * const item, const ChunkPositionVector & pos
	) : TerrainRaytraceResult(E_PlacedItem, item, pos)
	{}

	TerrainRaytraceResult::TerrainRaytraceResult() :
		TerrainRaytraceResult(E_None, NULL, { 0, 0 })
	{}
}

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
		ToFVector(TerrainGenParams->ToRealCoordSpace(itemData->Position.second)),
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
	CurrentChunkData = chunkData.Get(1, 1, 1);
	// Make sure the item ID counter is at least 1 larger than the current largest ID
	for (const auto & itemData : CurrentChunkData->PlacedItems) {
		if (itemData->ItemId >= ItemIdCounter)
			ItemIdCounter = itemData->ItemId + 1;
	}
	GenerateChunkMesh();
}

TerrainRaytraceResult AChunk::FindIntersection(const AxisAlignedBoundingBox3D & bound) {
	const Point3D position(gridIndex.X, gridIndex.Y, gridIndex.Z);
	// TODO: handle player collisions somehow
	if (SolidTerrain.Get(gridIndex.X, gridIndex.Y, gridIndex.Z)) {
		return TerrainRaytraceResult(
			this, ChunkPositionVector(CurrentChunkData->ChunkOffset, position));
	} else {
		// Check items to make sure nothing occupies this location. Perhaps using an octree
		// might be wise here.
		// TODO: make this work for arbitrarily placed items
		const double inset = 0.01; // Accounts for rounding errors
		const AxisAlignedBoundingBox3D bb(
			Point3D(gridIndex.X + inset, gridIndex.Y + inset, gridIndex.Z + inset),
			Point3D(gridIndex.X + 1 - inset, gridIndex.Y + 1 - inset, gridIndex.Z + 1 - inset));
		for (const auto & item : PlacedItems) {
			const auto & itemData = item.ItemActor->GetItemData();
			const OrientedBoundingBox3D obb(0., itemData->Size, itemData->GetPositionMatrix());
			if (bb.FindIntersection(itemData->GetBoundingBox(), false)) {
				return TerrainRaytraceResult(
					item.ItemActor, ChunkPositionVector(CurrentChunkData->ChunkOffset, position));
			}
		}
	}
	return TerrainRaytraceResult();
}

AItem * AChunk::CreateItem(const items::ItemDataPtr & itemData, const bool preserveId) {
	const auto & position = itemData->Position.second;
	ChunkGridIndexVector vec(
		Uint16(utils::EFloor(position.X)),
		Uint16(utils::EFloor(position.Y)),
		Uint16(utils::EFloor(position.Z)));
	if (FindIntersection(vec).Type == E_None) {
		if (!preserveId)
			itemData->ItemId = ItemIdCounter++;
		itemData->bIsPlaced = true;
		CurrentChunkData->PlacedItems.push_back(itemData);
		return SpawnItem(itemData);
	}
	return NULL;
}

void AChunk::GenerateChunkMesh() {
	auto material = UMaterialInstanceDynamic::Create((UMaterial *) TestMaterial, this);

	Uint32 cellCount = TerrainGenParams->GridCellCount;

	double scale = TerrainGenParams->ChunkScale / cellCount;

	std::vector<Triangle3D> tempTris;
	Vector3D<double> displacementVector;

	GridCell gridCell;
	Tensor3D<float> densityDataPoints(cellCount + 1);
	
	// Populate the density data
	for (Uint32 x = 0; x < cellCount + 1; x++) {
		for (Uint32 y = 0; y < cellCount + 1; y++) {
			for (Uint32 z = 0; z < cellCount + 1; z++) {
				ChunkDataPtr mainData =
					ChunkNeighbourData.Get(x / cellCount + 1, y / cellCount + 1, z / cellCount + 1);
				densityDataPoints.Set(
					x, y, z, mainData->DensityData.Get(
						x % cellCount, y % cellCount, z % cellCount));
			}
		}
	}

	std::vector<Triangle3D> triangles;

	// Build the mesh
	for (Uint32 x = 0; x < cellCount; x++) {
		for (Uint32 y = 0; y < cellCount; y++) {
			for (Uint32 z = 0; z < cellCount; z++) {
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

TerrainRaytraceResult AChunk::Raytrace(const Ray3D & ray, const double maxDistance) {
	FastVoxelTraversalIterator fvt(
		1.0, 0, TerrainGenParams->GridCellCount,
		ray, maxDistance / TerrainGenParams->ChunkGridUnitSize);

	TerrainRaytraceResultType foundType = E_None;

	for (Uint16 i = 0; fvt.IsValid(); i++) {
		const auto & current = fvt.GetCurrentCell();
		if (current.IsBoundedBy(0, TerrainGenParams->GridCellCount)) {
			const auto found = FindIntersection(current.Cast<Uint16>());
			if (found.Type != E_None) {
				const auto & collisionIndex = fvt.GetCurrentCell();
				const auto & precollisionIndex = fvt.GetPreviousCell();
				return TerrainRaytraceResult(
					found.Type, found.Result,
					ChunkPositionVector(CurrentChunkData->ChunkOffset, precollisionIndex.Cast<double>()));
			}
		}

		fvt.Next();
	}

	return TerrainRaytraceResult();
}

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
	ChunkGridIndexVector & result,
	const Ray3D & ray,
	const double maxDistance
) const {
	// Algorithm from here: http://www.cse.yorku.ca/~amana/research/grid.pdf
	result.Reset(0, 0, 0);

	/*const Point3D adjustedOrigin =
		TerrainGenParams->ToChunkCoordinates(ray.Origin, CurrentChunkData->ChunkOffset).second;*/
	
	const Point3D adjustedOrigin = ray.Origin;
	const auto gcc = TerrainGenParams->GridCellCount;

	const AxisAlignedBoundingBox3D boundingBox({ 0, 0, 0 }, { 0.9999999, 0.9999999, 0.9999999 });

	// Make sure the ray intersects the bounding box.
	Point3D entryPoint;
	double tEntry;
	const double tCheckRadius = maxDistance / TerrainGenParams->ChunkScale;
	const bool doesEnter = boundingBox.FindIntersection(
		Ray3D(adjustedOrigin, ray.Direction), &entryPoint, &tEntry);

	// If the ray doesn't enter the chunk within the maximum allowed t-value, then return false.
	if (!doesEnter || tEntry > tCheckRadius || tEntry < 0)
		return false;

	assert(!entryPoint.IsBoundedBy(0, 1) && "AChunk::SolidIntersection: Invalid entry point");

	ChunkGridIndexVector currentCell = TerrainGenParams->GetChunkGridIndicies(entryPoint);
	const Vector3D<Int8> step(
		Sign(ray.Direction.X),
		Sign(ray.Direction.Y),
		Sign(ray.Direction.Z));

	Vector3D<double> tMax(std::numeric_limits<double>::infinity());
	Vector3D<double> tDelta(0);

	// Initialize the current t value and the per-cell t deltas.
	for (Uint8 i = 0; i < 3; i++) {
		if (step[i] < 0)
			tMax[i] = (currentCell[i] / (double) gcc - entryPoint[i]) / ray.Direction[i];
		else if (step[i] > 0)
			tMax[i] = ((currentCell[i] + 1) / (double) gcc - entryPoint[i]) / ray.Direction[i];
		tDelta[i] = (1.0 / gcc) / std::abs(ray.Direction[i]);
	}

	bool isFound = false;

	do {
		if (tMax.X < tMax.Y) {
			if (tMax.X < tMax.Z) {
				if ((signed) currentCell.X < -step.X || currentCell.X + step.X >= gcc)
					break; // Not found.
				currentCell.X += step.X;
				tMax.X += tDelta.X;
			} else {
				if ((signed) currentCell.Z < -step.Z || currentCell.Z + step.Z >= gcc)
					break; // Not found.
				currentCell.Z += step.Z;
				tMax.Z += tDelta.Z;
			}
		} else {
			if (tMax.Y < tMax.Z) {
				if ((signed) currentCell.Y < -step.Y || currentCell.Y + step.Y >= gcc)
					break; // Not found.
				currentCell.Y += step.Y;
				tMax.Y += tDelta.Y;
			} else {
				if ((signed) currentCell.Z < -step.Z || currentCell.Z + step.Z >= gcc)
					break; // Not found.
				currentCell.Z += step.Z;
				tMax.Z += tDelta.Z;
			}
		}
		// Target is out of range
		if (tMax.X > tCheckRadius && tMax.Y > tCheckRadius && tMax.Z > tCheckRadius)
			break;
		// Stop when a solid block of terrain is found.
		isFound = SolidTerrain.Get(currentCell.X, currentCell.Y, currentCell.Z);
	} while (!isFound);

	if (isFound)
		result = currentCell;

	return isFound;
}

#include "Daedalus.h"
#include "ChunkManager.h"

#include <Models/Terrain/ChunkLoader.h>
#include <Utilities/FastVoxelTraversal.h>

using namespace utils;
using namespace events;
using namespace terrain;
using namespace items;

// TODO: pull out the item factory and data factory into a more global class
AChunkManager::AChunkManager(const class FPostConstructInitializeProperties & PCIP) :
	Super(PCIP), RenderDistance(1), TerrainInteractionDistance(250),
	ItemDataFactory(new items::ItemDataFactory())
{}

void AChunkManager::SetUpDefaultCursor() {
	const FRotator defaultRotator(0, 0, 0);
	auto params = FActorSpawnParameters();
	params.Name = TEXT("DummyCursor");
	auto itemData = ItemDataFactory->BuildItemData(I_Chest);
	DefaultCursor = GetWorld()->SpawnActor<AItem>(
		AItem::StaticClass(), { 0, 0, 0 }, { 0, 0, 0 }, params);
	DefaultCursor->Initialize(itemData);
	DefaultCursor->SetActorEnableCollision(false);
	CurrentCursor = DefaultCursor;
	CurrentCursor->SetActorHiddenInGame(true);
}

void AChunkManager::UpdateChunksAt(const utils::Vector3D<> & playerPosition) {
	ChunkOffsetVector offset;
	FRotator defaultRotation(0, 0, 0);
	
	FActorSpawnParameters defaultParameters;
	defaultParameters.Owner = this;

	// Get player's current chunk location
	auto playerChunkPos = GenParams->ToChunkCoordinates(playerPosition);

	Int64 fromX = playerChunkPos.first.X - RenderDistance;
	Int64 fromY = playerChunkPos.first.Y - RenderDistance;
	Int64 fromZ = playerChunkPos.first.Z - RenderDistance;

	Int64 toX = playerChunkPos.first.X + RenderDistance;
	Int64 toY = playerChunkPos.first.Y + RenderDistance;
	Int64 toZ = playerChunkPos.first.Z + RenderDistance;

	// Once the player leaves an area, the chunks are cleared
	for (auto chunkKey = LocalCache.begin(); chunkKey != LocalCache.end(); ) {
		if (chunkKey->first.X > toX || chunkKey->first.Y > toY ||
				chunkKey->first.Z > toZ || chunkKey->first.X < fromX ||
				chunkKey->first.Y < fromY || chunkKey->first.Z < fromZ) {
			chunkKey->second->Destroy();
			chunkKey = LocalCache.erase(chunkKey);
		} else {
			++chunkKey;
		}
	}

	// Begin preloading chunks for the area the player is near
	for (Int64 x = fromX; x <= toX; x++) {
		for (Int64 y = fromY; y <= toY; y++) {
			for (Int64 z = fromZ; z <= toZ; z++) {
				offset.Reset(x, y, z);
				GetChunkAt(offset);
			}
		}
	}
}

AChunk * AChunkManager::GetChunkAt(const ChunkOffsetVector & point) {
	FRotator defaultRotation(0, 0, 0);
	FActorSpawnParameters defaultParameters;
	defaultParameters.Owner = this;

	// Chunk is cached already
	if (LocalCache.count(point) > 0) {
		return LocalCache.at(point);
	} else {
		auto data = AChunk::ChunkDataSet();
		for (Uint8 x = 0; x < data.GetWidth(); x++) {
			for (Uint8 y = 0; y < data.GetDepth(); y++) {
				for (Uint8 z = 0; z < data.GetHeight(); z++) {
					data.Set(x, y, z, ChunkLoaderRef->GetChunkAt({
						point.X + x, point.Y + y, point.Z + z
					}));
				}
			}
		}
		auto position = ToFVector(GenParams->ToRealCoordSpace(point));

		//UE_LOG(LogTemp, Error, TEXT("Placing chunk at %f %f %f"), position.X, position.Y, position.Z)
		AChunk * newChunk = GetWorld()->SpawnActor<AChunk>(
			AChunk::StaticClass(), position, defaultRotation, defaultParameters);
		newChunk->InitializeChunk(GenParams);
		newChunk->SetChunkData(data);
		newChunk->AttachRootComponentToActor(this);
		LocalCache.insert({ point, newChunk });
		return newChunk;
	}
}

void AChunkManager::UpdateItemPosition(AItem * item, const ChunkPositionVector & position) {
	auto chunk = GetChunkAt(position.first);
	item->AttachRootComponentToActor(chunk);
	item->SetPosition(position);
}

void AChunkManager::UpdateCursorPosition(const Ray3D & viewpoint) {
	const auto foundResult = Raytrace(viewpoint, TerrainInteractionDistance);

	if (foundResult.Type == E_Terrain || foundResult.Type == E_PlacedItem) {
		const Int32 gcc = GenParams->GridCellCount;
		const double x = foundResult.EntryPosition.second.X;
		const double y = foundResult.EntryPosition.second.Y;
		const double z = foundResult.EntryPosition.second.Z;
		const Vector3D<Int64> offset(
			std::floor(x / gcc),
			std::floor(y / gcc),
			std::floor(z / gcc));
		const Point3D itemPosition(
			x - offset.X * gcc,
			y - offset.Y * gcc,
			z - offset.Z * gcc);
		UpdateItemPosition(CurrentCursor, ChunkPositionVector(
			foundResult.EntryPosition.first + offset, itemPosition));
		CurrentCursor->SetActorHiddenInGame(false);
	} else {
		CurrentCursor->SetActorHiddenInGame(true);
	}
}

void AChunkManager::UpdateCursorRotation(const Point2D & rotationOffset) {
	if (CurrentCursor) {
		const auto & data = CurrentCursor->GetItemData();
		ItemRotation rot(
			rotationOffset.X / 360.0 * data->Template.RotationInterval.Yaw,
			rotationOffset.Y / 360.0 * data->Template.RotationInterval.Pitch);
		CurrentCursor->SetRotation(rot);
	}
}

TerrainRaytraceResult AChunkManager::Raytrace(
	const utils::Ray3D & viewpoint,
	const double maxDist
) {
	FastVoxelTraversalIterator fvt(GenParams->ChunkScale, viewpoint, maxDist);

	bool foundIntersect = false;
	Vector3D<Int64> chunkPosition = fvt.GetCurrentCell();

	// Search through adjacent chunks
	while (fvt.IsValid()) {
		// Get current chunk location
		const auto chunkPos = GenParams->ToChunkCoordinates(viewpoint.Origin, chunkPosition);
		auto chunk = GetChunkAt(chunkPos.first);
		const auto result = chunk->Raytrace(
			Ray3D(chunkPos.second, viewpoint.Direction),
			TerrainInteractionDistance);

		if (result.Type != E_None)
			return result;

		// If no intersections are found, we need to search adjacent chunks.
		fvt.Next();
		chunkPosition = fvt.GetCurrentCell();
	}

	return TerrainRaytraceResult();
}

void AChunkManager::PlaceItem() {
	// TODO: visual effect for finalizing or cancelling item placement
	if (CurrentCursor && !CurrentCursor->bHidden) {
		auto & cursorItemData = CurrentCursor->GetItemData();
		auto chunk = GetChunkAt(cursorItemData->Position.first);
		auto newItemData = ItemDataPtr(new ItemData(*cursorItemData));
		chunk->CreateItem(newItemData);
	}
}

void AChunkManager::BeginPlay() {
	Super::BeginPlay();

	ChunkLoaderRef = GetGameState()->ChunkLoader;
	GenParams = &ChunkLoaderRef->GetGeneratorParameters();
	EventBusRef = GetGameState()->EventBus;

	SetUpDefaultCursor();
	EventBusRef->AddListener(E_PlayerPosition, this);
	EventBusRef->AddListener(E_ViewPosition, this);
	EventBusRef->AddListener(E_FPItemPlacementBegin, this);
	EventBusRef->AddListener(E_FPItemPlacementEnd, this);
	EventBusRef->AddListener(E_FPItemPlacementRotation, this);
}

void AChunkManager::HandleEvent(const EventDataPtr & data) {
	switch (data->Type) {
	case E_ViewPosition: {
		auto castedData = std::static_pointer_cast<EViewPosition>(data);
		UpdateCursorPosition(Ray3D(castedData->ViewOrigin, castedData->ViewDirection));
		break;
	}
	case E_PlayerPosition: {
		auto castedData = std::static_pointer_cast<EPlayerPosition>(data);
		//UE_LOG(LogTemp, Warning, TEXT("Player position: %f %f %f"), position.X, position.Y, position.Z);
		UpdateChunksAt(castedData->Position);
		break;
	}
	case E_FPItemPlacementBegin: {
		//auto castedData = std::static_pointer_cast<EFPItemPlacementBegin>(data);
		// TODO: visual effect for beginning item placement
		break;
	}
	case E_FPItemPlacementEnd: {
		auto castedData = std::static_pointer_cast<EFPItemPlacementEnd>(data);
		if (!castedData->bIsCancelled)
			PlaceItem();
		break;
	}
	case E_FPItemPlacementRotation: {
		auto castedData = std::static_pointer_cast<EFPItemPlacementRotation>(data);
		UpdateCursorRotation(castedData->RotationOffset);
		break;
	}
	}
}

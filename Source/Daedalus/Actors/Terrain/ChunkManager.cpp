#include "Daedalus.h"
#include "ChunkManager.h"

#include <Actors/Items/ItemFactory.h>
#include <Models/Terrain/ChunkLoader.h>

using namespace utils;
using namespace events;
using namespace terrain;
using namespace items;

AChunkManager::AChunkManager(const class FPostConstructInitializeProperties & PCIP) :
	Super(PCIP), RenderDistance(1), TerrainInteractionDistance(150),
	ItemFactory(NewObject<UItemFactory>(this, UItemFactory::StaticClass()))
{}

void AChunkManager::SetUpDefaultCursor() {
	const FRotator defaultRotator(0, 0, 0);
	auto params = FActorSpawnParameters();
	params.Name = TEXT("DummyCursor");
	auto itemData = ItemDataPtr(new ItemData(I_Chest, ItemDataId(0, { 0, 0 })));
	DefaultCursor = GetWorld()->SpawnActor<AItem>(
		ItemFactory->GetItemClass(itemData),
		{ 0, 0, 0 }, defaultRotator, params);
	CurrentCursor = DefaultCursor;
	CurrentCursor->SetActorHiddenInGame(true);
	CurrentCursor->SetActorEnableCollision(false);
}

void AChunkManager::UpdateChunksAt(const utils::Vector3D<> & playerPosition) {
	ChunkOffsetVector offset;
	FRotator defaultRotation(0, 0, 0);
	auto genParams = GetGameState()->ChunkLoader->GetGeneratorParameters();
	
	FActorSpawnParameters defaultParameters;
	defaultParameters.Owner = this;

	// Get player's current chunk location
	auto playerChunkPos = genParams.ToChunkCoordinates(playerPosition);

	Int64 fromX = playerChunkPos.first.X - RenderDistance;
	Int64 fromY = playerChunkPos.first.Y - RenderDistance;
	Int64 fromZ = playerChunkPos.first.Z - RenderDistance;

	Int64 toX = playerChunkPos.first.X + RenderDistance;
	Int64 toY = playerChunkPos.first.Y + RenderDistance;
	Int64 toZ = playerChunkPos.first.Z + RenderDistance;

	// Once the player leaves an area, the chunks are cleared
	for (auto chunkKey = LocalCache.begin(); chunkKey != LocalCache.end(); ) {
		if (chunkKey->first.X > toX || chunkKey->first.Y > toY || chunkKey->first.Z > toZ ||
			chunkKey->first.X < fromX || chunkKey->first.Y < fromY || chunkKey->first.Z < fromZ) {
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
	auto chunkLoader = GetGameState()->ChunkLoader;
	auto genParams = chunkLoader->GetGeneratorParameters();
	
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
					data.Set(x, y, z, chunkLoader->GetChunkAt({
						point.X + x, point.Y + y, point.Z + z
					}));
				}
			}
		}
		auto position = ToFVector(genParams.ToRealCoordinates(point));

		//UE_LOG(LogTemp, Error, TEXT("Placing chunk at %f %f %f"), position.X, position.Y, position.Z)
		AChunk * newChunk = GetWorld()->SpawnActor<AChunk>(
			AChunk::StaticClass(), position, defaultRotation, defaultParameters);
		newChunk->InitializeChunk(genParams, ItemFactory);
		newChunk->SetChunkData(data);
		LocalCache.insert({ point, newChunk });
		return newChunk;
	}
}

void AChunkManager::UpdateCursorPosition(const Ray3D & viewpoint) {
	auto genParams = GetGameState()->ChunkLoader->GetGeneratorParameters();

	// Get player's current chunk location
	auto playerChunkPos = genParams.ToChunkCoordinates(viewpoint.Origin);
	auto chunk = GetChunkAt(playerChunkPos.first);
	ChunkGridIndexVector indexVec;
	bool foundIntersect = chunk->TerrainIntersection(
		indexVec, { playerChunkPos.second, viewpoint.Direction }, TerrainInteractionDistance);
	if (foundIntersect) {
		indexVec.Z += 1;
		const auto newPosition = ToFVector(genParams.GetChunkInnerPosition(indexVec));
		CurrentCursor->SetActorHiddenInGame(false);
		CurrentCursor->SetActorRelativeLocation(newPosition);
		CurrentCursor->AttachRootComponentToActor(chunk);
		// Found an intersection at the current chunk position.
	} else {
		// If no intersections are found, we need to search adjacent chunks.
		CurrentCursor->SetActorHiddenInGame(true);
	}
}

void AChunkManager::BeginPlay() {
	Super::BeginPlay();
	SetUpDefaultCursor();
	GetGameState()->EventBus->AddListener(E_PlayerPosition, this);
	GetGameState()->EventBus->AddListener(E_ViewPosition, this);
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
	}
}

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

void AChunkManager::UpdateChunksAt(const utils::Vector3D<> & playerPosition) {
	ChunkOffsetVector offset;
	FRotator defaultRotation(0, 0, 0);
	
	FActorSpawnParameters defaultParameters;
	defaultParameters.Owner = this;

	// Get player's current chunk location
	auto playerChunkPos = GenParams->ToGridCoordSpace(playerPosition);

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
		Uint8 w = data.GetWidth(), d = data.GetDepth(), h = data.GetHeight();
		for (Uint8 x = 0; x < w; x++) {
			for (Uint8 y = 0; y < d; y++) {
				for (Uint8 z = 0; z < h; z++) {
					data.Set(x, y, z, ChunkLoaderRef->GetChunkAt({
						point.X + x - w / 2, point.Y + y - d / 2, point.Z + z - h / 2
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
		const auto chunkPos = GenParams->ToGridCoordSpace(viewpoint.Origin, chunkPosition);
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

AItem * AChunkManager::PlaceItem(const items::ItemDataPtr & data) {
	auto chunk = GetChunkAt(data->Position.first);
	return chunk->CreateItem(data);
}

void AChunkManager::BeginPlay() {
	Super::BeginPlay();

	ChunkLoaderRef = GetGameState()->ChunkLoader;
	GenParams = &ChunkLoaderRef->GetGeneratorParameters();
	EventBusRef = GetGameState()->EventBus;

	EventBusRef->AddListener(E_PlayerPosition, this);
	EventBusRef->AddListener(E_ViewPosition, this);
}

void AChunkManager::HandleEvent(const EventDataPtr & data) {
	switch (data->Type) {
	case E_PlayerPosition: {
		auto castedData = std::static_pointer_cast<EPlayerPosition>(data);
		//UE_LOG(LogTemp, Warning, TEXT("Player position: %f %f %f"), position.X, position.Y, position.Z);
		UpdateChunksAt(castedData->Position);
		break;
	}
	}
}

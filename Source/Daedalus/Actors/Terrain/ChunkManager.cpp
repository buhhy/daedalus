#include "Daedalus.h"
#include "ChunkLoader.h"
#include "ChunkManager.h"

AChunkManager::AChunkManager(
	const class FPostConstructInitializeProperties & PCIP
) : Super(PCIP), RenderDistance(2) {}

ADDGameState * AChunkManager::GetGameState() {
	return GetWorld()->GetGameState<ADDGameState>();
}

void AChunkManager::UpdateChunksAt(const utils::Vector3<> & playerPosition) {
	terrain::ChunkOffsetVector offset;
	FRotator defaultRotation(0, 0, 0);
	FActorSpawnParameters defaultParameters;
	auto chunkLoader = GetGameState()->ChunkLoader;
	auto genParams = chunkLoader->GetGeneratorParameters();

	// Get player's current chunk location
	auto playerChunkPos = genParams.ToChunkCoordinates(playerPosition);

	int64_t fromX = playerChunkPos.X - RenderDistance;
	int64_t fromY = playerChunkPos.Y - RenderDistance;
	int64_t fromZ = playerChunkPos.Z - RenderDistance;

	int64_t toX = playerChunkPos.X + RenderDistance;
	int64_t toY = playerChunkPos.Y + RenderDistance;
	int64_t toZ = playerChunkPos.Z + RenderDistance;

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
	for (int64_t x = fromX; x <= toX; x++) {
		for (int64_t y = fromY; y <= toY; y++) {
			for (int64_t z = fromZ; z <= toZ; z++) {
				offset.Reset(x, y, z);
				// Chunk is cached already
				if (LocalCache.count(offset) > 0) {
				} else {
					auto data = chunkLoader->GetChunkAt(offset);
					auto position = utils::ToFVector(
						genParams.ToRealCoordinates(data->ChunkOffset));

					//UE_LOG(LogTemp, Error, TEXT("Placing chunk at %f %f %f"), position.X, position.Y, position.Z)
					AChunk * newChunk = GetWorld()->SpawnActor<AChunk>(
						AChunk::StaticClass(), position, defaultRotation, defaultParameters);
					newChunk->InitializeChunk(genParams);
					newChunk->SetChunkData(data);
					LocalCache.insert({ offset, newChunk });
				}
			}
		}
	}
}

void AChunkManager::BeginPlay() {
	Super::BeginPlay();
	GetGameState()->EventBus->AddListener(events::E_PlayerMovement, this);
}

void AChunkManager::HandleEvent(
	const events::EventType type,
	const std::shared_ptr<events::EventData> & data
) {
	auto castedData = std::static_pointer_cast<events::EPlayerMovement>(data);
	//UE_LOG(LogTemp, Warning, TEXT("Player position: %f %f %f"), position.X, position.Y, position.Z);
	UpdateChunksAt(castedData->Position);
}

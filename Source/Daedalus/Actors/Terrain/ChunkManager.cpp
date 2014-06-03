#include "Daedalus.h"
#include "ChunkLoader.h"
#include "ChunkManager.h"

AChunkManager::AChunkManager(
	const class FPostConstructInitializeProperties & PCIP
) : Super(PCIP) {
	RenderDistance = 3;
}

ADDGameState * AChunkManager::GetGameState() {
	return GetWorld()->GetGameState<ADDGameState>();
}

void AChunkManager::UpdateChunksAt(const FVector & playerPosition) {
	ChunkOffset offset;
	FRotator defaultRotation(0, 0, 0);
	FActorSpawnParameters defaultParameters;
	auto chunkLoader = GetGameState()->ChunkLoader;
	auto genParams = chunkLoader->GetGeneratorParameters();

	// Get player's current chunk location
	auto playerChunkPos = genParams.ToChunkCoordinates(playerPosition);

	int64 fromX = playerChunkPos.X - RenderDistance;
	int64 fromY = playerChunkPos.Y - RenderDistance;
	int64 fromZ = playerChunkPos.Z - RenderDistance;

	int64 toX = playerChunkPos.X + RenderDistance;
	int64 toY = playerChunkPos.Y + RenderDistance;
	int64 toZ = playerChunkPos.Z + RenderDistance;

	for (int64 x = fromX; x <= toX; x++) {
		for (int64 y = fromY; y <= toY; y++) {
			for (int64 z = fromZ; z <= toZ; z++) {
				offset.Reset(x, y, z);
				// Chunk is cached already
				if (LocalCache.count(offset) > 0) {
				} else {
					auto data = chunkLoader->GetChunkAt(offset);
					auto position = genParams.ToRealCoordinates(data->ChunkOffset);

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
	const TSharedRef<events::EventData> & data
) {
	auto castedData = StaticCastSharedRef<events::EPlayerMovement>(data);
	auto position = castedData->Source->GetActorLocation();
	//UE_LOG(LogTemp, Warning, TEXT("Player position: %f %f %f"), position.X, position.Y, position.Z);
	UpdateChunksAt(position);
}

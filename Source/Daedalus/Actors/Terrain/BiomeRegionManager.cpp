#include "Daedalus.h"
#include "ChunkLoader.h"
#include "BiomeRegionManager.h"


inline FVector2D To2D(const FVector & in) { return { in.X, in.Y }; }
inline FVector To3D(const FVector2D & in, const float z) { return { in.X, in.Y, z }; }

ABiomeRegionManager::ABiomeRegionManager(
	const class FPostConstructInitializeProperties & PCIP
) : Super(PCIP), RenderDistance(2), RenderHeight(100.0) {}

ADDGameState * ABiomeRegionManager::GetGameState() {
	return GetWorld()->GetGameState<ADDGameState>();
}

void ABiomeRegionManager::UpdateBiomesAt(const FVector & playerPosition) {
	terrain::BiomeOffsetVector offset;
	FRotator defaultRotation(0, 0, 0);
	FActorSpawnParameters defaultParameters;
	auto chunkLoader = GetGameState()->BiomeRegionLoader;
	auto genParams = chunkLoader->GetGeneratorParameters();

	// Get player's current chunk location
	auto playerChunkPos = genParams.ToChunkCoordinates(To2D(playerPosition));

	int64 fromX = playerChunkPos.X - RenderDistance;
	int64 fromY = playerChunkPos.Y - RenderDistance;

	int64 toX = playerChunkPos.X + RenderDistance;
	int64 toY = playerChunkPos.Y + RenderDistance;

	// Once the player leaves an area, the chunks are cleared
	for (auto chunkKey = LocalCache.begin(); chunkKey != LocalCache.end(); ) {
		if (chunkKey->first.X > toX || chunkKey->first.Y > toY ||
			chunkKey->first.X < fromX || chunkKey->first.Y < fromY) {

			chunkKey->second->Destroy();
			chunkKey = LocalCache.erase(chunkKey);
		} else {
			++chunkKey;
		}
	}

	// Begin preloading biomes for the area the player is near
	for (int64 x = fromX; x <= toX; x++) {
		for (int64 y = fromY; y <= toY; y++) {
			offset.Reset(x, y);
			// Biome is cached already
			if (LocalCache.count(offset) > 0) {
			} else {
				auto data = chunkLoader->GetBiomeRegionAt(offset);
				auto position = To3D(genParams.ToRealCoordinates(data->BiomeOffset), RenderHeight);

				//UE_LOG(LogTemp, Error, TEXT("Placing chunk at %f %f %f"), position.X, position.Y, position.Z)
				ABiomeRegion * newRegion = GetWorld()->SpawnActor<ABiomeRegion>(
					ABiomeRegion::StaticClass(), position, defaultRotation, defaultParameters);
				newRegion->InitializeBiomeRegion(genParams.BiomeScale);
				newRegion->SetBiomeRegionData(data);
				LocalCache.insert({ offset, newRegion });
			}
		}
	}
}

void ABiomeRegionManager::BeginPlay() {
	Super::BeginPlay();
	GetGameState()->EventBus->AddListener(events::E_PlayerMovement, this);
}

void ABiomeRegionManager::HandleEvent(
	const events::EventType type,
	const TSharedRef<events::EventData> & data
) {
	auto castedData = StaticCastSharedRef<events::EPlayerMovement>(data);
	auto position = castedData->Source->GetActorLocation();
	//UE_LOG(LogTemp, Warning, TEXT("Player position: %f %f %f"), position.X, position.Y, position.Z);
	UpdateBiomesAt(position);
}

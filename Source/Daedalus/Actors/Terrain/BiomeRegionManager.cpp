#include <Daedalus.h>
#include "BiomeRegionManager.h"

#include <Models/Terrain/ChunkLoader.h>
#include <Utilities/UnrealBridge.h>

using namespace utils;
using namespace events;

ABiomeRegionManager::ABiomeRegionManager(
	const class FPostConstructInitializeProperties & PCIP
) : Super(PCIP), RenderDistance(0), RenderHeight(100.0) {}

ADDGameState * ABiomeRegionManager::GetGameState() {
	return GetWorld()->GetGameState<ADDGameState>();
}

void ABiomeRegionManager::UpdateBiomesAt(const Vector3D<> & playerPosition) {
	terrain::BiomeRegionOffsetVector offset;
	auto chunkLoader = GetGameState()->BiomeRegionLoader;
	auto genParams = chunkLoader->GetGeneratorParameters();

	// Get player's current chunk location
	auto const & playerChunkPos =
		genParams.ToBiomeRegionCoordinates(playerPosition.Truncate()).first;

	int64 fromX = playerChunkPos.X - RenderDistance;
	int64 fromY = playerChunkPos.Y - RenderDistance;

	int64 toX = playerChunkPos.X + RenderDistance;
	int64 toY = playerChunkPos.Y + RenderDistance;

	// Once the player leaves an area, the chunks are cleared
	/*for (auto chunkKey = LocalCache.begin(); chunkKey != LocalCache.end(); ) {
		if (chunkKey->first.X > toX || chunkKey->first.Y > toY ||
			chunkKey->first.X < fromX || chunkKey->first.Y < fromY) {

			chunkKey->second->Destroy();
			chunkKey = LocalCache.erase(chunkKey);
		} else {
			++chunkKey;
		}
	}*/

	// Begin preloading biomes for the area the player is near
	for (int64 x = fromX; x <= toX; x++) {
		for (int64 y = fromY; y <= toY; y++) {
			offset.Reset(x, y);
			// Biome is cached already
			if (LocalCache.count(offset) > 0) {
			} else {
				ReloadRegionAt(offset);
			}
		}
	}
}

void ABiomeRegionManager::ReloadRegionAt(const terrain::BiomeRegionOffsetVector & offset) {
	auto chunkLoader = GetGameState()->BiomeRegionLoader;
	auto genParams = chunkLoader->GetGeneratorParameters();
	auto data = chunkLoader->GetBiomeRegionAt(offset);
	auto position = ToFVector(
		genParams.ToRealCoordSpace(data->GetBiomeRegionOffset()), RenderHeight);
	FRotator defaultRotation(0, 0, 0);
	FActorSpawnParameters defaultParameters;

	UE_LOG(LogTemp, Error, TEXT("Placing biome at %f %f %f"), position.X, position.Y, position.Z)
	ABiomeRegion * newRegion = GetWorld()->SpawnActor<ABiomeRegion>(
		ABiomeRegion::StaticClass(), position, defaultRotation, defaultParameters);
	newRegion->InitializeBiomeRegion(genParams.BiomeScale);
	newRegion->SetBiomeRegionData(data);
	LocalCache.insert({ offset, newRegion });
}

bool ABiomeRegionManager::DeleteRegionAt(const terrain::BiomeRegionOffsetVector & offset) {
	if (LocalCache.count(offset) > 0) {
		LocalCache.at(offset)->Destroy();
		LocalCache.erase(offset);
		return true;
	} else {
		return false;
	}
}

void ABiomeRegionManager::BeginPlay() {
	Super::BeginPlay();
	//GetGameState()->EventBus->AddListener(events::E_PlayerPosition, this);
	//GetGameState()->EventBus->AddListener(events::E_BiomeRegionUpdate, this);
}

void ABiomeRegionManager::HandleEvent(const EventDataPtr & data) {
	if (data->Type == events::E_PlayerPosition) {
		auto castedData = std::static_pointer_cast<events::EPlayerPosition>(data);
		//UE_LOG(LogTemp, Warning, TEXT("Player position: %f %f %f"), position.X, position.Y, position.Z);
		UpdateBiomesAt(castedData->Position);
	} else if (data->Type == events::E_BiomeRegionUpdate) {
		auto castedData = std::static_pointer_cast<events::EBiomeRegionUpdate>(data);
		for (auto offset : castedData->UpdatedOffsets) {
			if (DeleteRegionAt(offset))
				ReloadRegionAt(offset);
		}
	}
}

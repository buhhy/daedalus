#include "Daedalus.h"
#include "DDGameState.h"
#include "ChunkManager.h"

AChunkManager::AChunkManager(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP), Seed(12345678), ChunkSize(17, 17, 17)
{
}

void AChunkManager::BeginPlay() {
	Super::BeginPlay();
	//auto gameWorld = GetWorld()->GameState;
	//auto count = GetWorld()->GetGameState<ADDGameState>()->EventBus->Count(events::E_PlayerMovement);
	//UE_LOG(LogTemp, Warning, TEXT("Chunk manager init: %d"), count);
	GetWorld()->GetGameState<ADDGameState>()->EventBus->AddListener(
		events::E_PlayerMovement, this);
}

void AChunkManager::HandleEvent(
	const events::EventType type,
	const TSharedRef<events::EventData> & data
) {
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("Received event!"));
}

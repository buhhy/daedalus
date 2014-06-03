#include "Daedalus.h"
#include "DDGameState.h"

ADDGameState::ADDGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP),
	TerrainGenParams({ { 17, 17, 17 }, 12345678U, 20.0 }),
	ChunkLoader(new terrain::ChunkLoader(TerrainGenParams))
{
	EventBus = NewObject<UEventBus>();
}

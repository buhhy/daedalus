#include <unordered_map>

#include "EventListener.h"
#include "DataStructures.h"
#include "DDGameState.h"
#include "TerrainDataStructures.h"
#include "Chunk.h"
#include "ChunkManager.generated.h"

/**
 * This class takes data fetched from the ChunkLoader and renders it. It is
 * responsible for knowing when to load and dispose of rendered chunks. This
 * will most likely run on the client-side.
 */
UCLASS()
class AChunkManager : public AActor, public IEventListener {
	GENERATED_UCLASS_BODY()

private:
	typedef std::unordered_map <
		terrain::ChunkOffsetVector, AChunk *
	> ChunkCache;

	ChunkCache LocalCache;
	uint64 RenderDistance;

	ADDGameState * GetGameState();
	void UpdateChunksAt(const FVector & playerPosition);

public:
	virtual void HandleEvent(
		const events::EventType type,
		const TSharedRef<events::EventData> & data);

	virtual void BeginPlay();
};

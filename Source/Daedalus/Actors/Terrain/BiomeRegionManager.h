#include <unordered_map>

#include "EventListener.h"
#include "DataStructures.h"
#include "DDGameState.h"
#include "TerrainDataStructures.h"
#include "BiomeRegion.h"
#include "BiomeRegionManager.generated.h"

/**
 * This manages biome rendering for debugging purposes.
 */
UCLASS()
class ABiomeRegionManager : public AActor, public IEventListener {
	GENERATED_UCLASS_BODY()

private:
	typedef std::unordered_map <
		terrain::BiomeOffsetVector, ABiomeRegion *
	> BiomeRegionCache;

	BiomeRegionCache LocalCache;
	uint64 RenderDistance;
	float RenderHeight;

	ADDGameState * GetGameState();
	void UpdateBiomesAt(const FVector & playerPosition);

public:
	virtual void HandleEvent(
		const events::EventType type,
		const TSharedRef<events::EventData> & data);

	virtual void BeginPlay();
};

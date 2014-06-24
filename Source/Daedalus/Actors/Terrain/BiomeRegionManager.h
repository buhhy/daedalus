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
		terrain::BiomeRegionOffsetVector, ABiomeRegion *
	> BiomeRegionCache;

	BiomeRegionCache LocalCache;
	uint64 RenderDistance;
	float RenderHeight;

	ADDGameState * GetGameState();
	void UpdateBiomesAt(const FVector & playerPosition);
	void ReloadRegionAt(const terrain::BiomeRegionOffsetVector & offset);
	void DeleteRegionAt(const terrain::BiomeRegionOffsetVector & offset);

public:
	virtual void HandleEvent(
		const events::EventType type,
		const TSharedRef<events::EventData> & data);

	virtual void BeginPlay();
};

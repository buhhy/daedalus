#include <unordered_map>

#include "EventListener.h"
#include "DataStructures.h"
#include "DDGameState.h"
#include "TerrainDataStructures.h"
#include "BiomeRegion.h"

#include <memory>

#include "BiomeRegionManager.generated.h"

/**
 * This manages biome rendering for debugging purposes.
 */
UCLASS()
class ABiomeRegionManager : public AActor, public EventListener {
	GENERATED_UCLASS_BODY()

private:
	typedef std::unordered_map <
		terrain::BiomeRegionOffsetVector, ABiomeRegion *
	> BiomeRegionCache;

	BiomeRegionCache LocalCache;
	uint64 RenderDistance;
	float RenderHeight;

	ADDGameState * GetGameState();
	void UpdateBiomesAt(const utils::Vector3D<> & playerPosition);
	void ReloadRegionAt(const terrain::BiomeRegionOffsetVector & offset);
	bool DeleteRegionAt(const terrain::BiomeRegionOffsetVector & offset);

public:
	virtual void HandleEvent(
		const events::EventType type,
		const std::shared_ptr<events::EventData> & data) override;

	virtual void BeginPlay() override;
};

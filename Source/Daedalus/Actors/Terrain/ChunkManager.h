#include <unordered_map>

#include "EventListener.h"
#include "DataStructures.h"
#include "Chunk.h"
#include "ChunkManager.generated.h"

/**
 * 
 */
UCLASS()
class AChunkManager : public AActor, public IEventListener {
	GENERATED_UCLASS_BODY()

private:
	uint64 Seed;
	utils::Vector3<uint64> ChunkSize;
	//std::unordered_map<utils::Vector3<int64>, TWeakObjectPtr<Chunk> > 

public:
	virtual void HandleEvent(
		const events::EventType type,
		const TSharedRef<events::EventData> & data);

	virtual void BeginPlay();
};

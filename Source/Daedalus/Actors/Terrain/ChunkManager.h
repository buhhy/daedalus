#include "EventListener.h"
#include "ChunkManager.generated.h"

UCLASS()
class AChunkManager : public AActor, public IEventListener {
	GENERATED_UCLASS_BODY()
public:
	virtual void HandleEvent(
		const events::EventType type,
		const TSharedRef<events::EventData> & data);

	virtual void BeginPlay();
};

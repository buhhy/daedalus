#pragma once

#include <Controllers/DDGameState.h>
#include <Controllers/ResourceCache.h>

#include "DDActor.generated.h"

UCLASS()
class ADDActor : public AActor {
	GENERATED_UCLASS_BODY()
protected:
	ADDGameState * getGameState() { return GetWorld()->GetGameState<ADDGameState>(); }
	ResourceCacheCPtr getResourceCache() { return getGameState()->getResourceCacheRef(); }
};

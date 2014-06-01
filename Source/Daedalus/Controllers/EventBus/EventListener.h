#pragma once

#include "Events.h"
#include "EventListener.generated.h"

UINTERFACE()
class UEventListener : public UInterface {
	GENERATED_UINTERFACE_BODY()
};

class IEventListener {
	GENERATED_IINTERFACE_BODY()
public:
	virtual void HandleEvent(
		const events::EventType type,
		const TSharedRef<events::EventData> & data) = 0;
};

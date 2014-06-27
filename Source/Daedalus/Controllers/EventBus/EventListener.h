#pragma once

#include "Events.h"

#include <memory>

class EventListener {
public:
	virtual void HandleEvent(
		const events::EventType type,
		const std::shared_ptr<events::EventData> & data) = 0;
};

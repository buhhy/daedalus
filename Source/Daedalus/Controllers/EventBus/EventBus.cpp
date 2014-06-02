#include "Daedalus.h"
#include "EventBus.h"

#include <algorithm>

UEventBus::UEventBus(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP) {
	Listeners.clear();
}

void UEventBus::AddListener(
	const events::EventType type,
	IEventListener * const listener
) {
	// Insert a list if no listeners are currently listening on this event
	if (Listeners.count(type) == 0) {
		Listeners.insert({
			type,
			TSharedRef<events::ListenerList>(new events::ListenerList())
		});
	}

	TSharedRef<events::ListenerList> & listeners = Listeners.at(type);

	bool found = false;

	// Look for duplicates and remove invalid pointers
	for (auto it = listeners->cbegin(); it != listeners->cend();) {
		if (*it == listener)
			found = true;
		++it;
	}

	if (!found)
		listeners->push_back(listener);
}

void UEventBus::RemoveListener(
	const events::EventType type,
	IEventListener * const listener
) {
	if (Listeners.count(type) > 0) {
		bool found = false;

		auto & listeners = Listeners.at(type);

		std::remove_if(
			listeners->begin(),
			listeners->end(),
			[&] (IEventListener *& x) { return x == listener; });
	}
}

uint32 UEventBus::Count(const events::EventType type) {
	if (Listeners.count(type) > 0)
		return Listeners.at(type)->size();
	return 0;
}

uint32 UEventBus::BroadcastEvent(
	const events::EventType type,
	const TSharedRef<events::EventData> & data
) {
	uint32 broadcastCount = 0;

	if (Listeners.count(type) > 0) {
		auto & listeners = Listeners.at(type);

		for (auto it = listeners->cbegin(); it != listeners->cend();) {
			(*it)->HandleEvent(type, data);
			++broadcastCount;
			++it;
		}
	}

	return broadcastCount;
}

void UEventBus::BeginDestroy() {
	for (auto it = Listeners.begin(); it != Listeners.end(); ++it)
		it->second->clear();

	Listeners.clear();
}

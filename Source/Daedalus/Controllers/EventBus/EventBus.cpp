#include "Daedalus.h"
#include "EventBus.h"

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
			TUniquePtr<events::ListenerList>(new events::ListenerList())
		});
	}

	TUniquePtr<events::ListenerList> & listeners = Listeners.at(type);

	bool found = false;

	// Look for duplicates and remove invalid pointers
	for (auto it = listeners->cbegin(); it != listeners->cend();) {
		if (*it == NULL) {
			it = listeners->erase(it);
		} else {
			if (*it == listener)
				found = true;
			++it;
		}
	}

	if (!found)
		listeners->push_back(listener);
}

bool UEventBus::RemoveListener(
	const events::EventType type,
	IEventListener * const listener
) {
	if (Listeners.count(type) > 0) {
		bool found = false;

		auto & listeners = Listeners.at(type);

		for (auto it = listeners->cbegin(); it != listeners->cend();) {
			if (*it == NULL) {
				it = listeners->erase(it);
			} else if (*it == listener) {
				it = listeners->erase(it);
				found = true;
			} else {
				++it;
			}
		}

		return found;
	}

	return false;
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
			if (*it == NULL) {
				it = listeners->erase(it);
			} else {
				(*it)->HandleEvent(type, data);
				++broadcastCount;
				++it;
			}
		}
	}

	return broadcastCount;
}

void UEventBus::BeginDestroy() {
	Super::BeginDestroy();

	for (auto it = Listeners.begin(); it != Listeners.end(); ++it) {
		for (uint64 i = 0; i < it->second->size(); i++)
			(*it->second)[i] = NULL;
		(it->second)->clear();
		it->second.Reset();
	}

	Listeners.clear();
}

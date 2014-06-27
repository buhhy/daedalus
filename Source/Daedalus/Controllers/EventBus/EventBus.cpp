#include "Daedalus.h"
#include "EventBus.h"

#include <algorithm>

namespace events {
	EventBus::EventBus() {
		Listeners.clear();
	}

	void EventBus::AddListener(
		const events::EventType type,
		EventListener * const listener
	) {
		// Insert a list if no listeners are currently listening on this event
		if (Listeners.count(type) == 0) {
			Listeners.insert({
				type,
				std::shared_ptr<events::ListenerList>(new events::ListenerList())
			});
		}

		std::shared_ptr<events::ListenerList> & listeners = Listeners.at(type);

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

	void EventBus::RemoveListener(
		const events::EventType type,
		EventListener * const listener
	) {
		if (Listeners.count(type) > 0) {
			bool found = false;

			auto & listeners = Listeners.at(type);

			std::remove_if(
				listeners->begin(),
				listeners->end(),
				[&] (EventListener *& x) { return x == listener; });
		}
	}

	uint32_t EventBus::Count(const events::EventType type) const {
		if (Listeners.count(type) > 0)
			return Listeners.at(type)->size();
		return 0;
	}

	uint32_t EventBus::BroadcastEvent(
		const events::EventType type,
		const std::shared_ptr<events::EventData> & data
	) {
		uint32_t broadcastCount = 0;

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

	EventBus::~EventBus() {
		for (auto it = Listeners.begin(); it != Listeners.end(); ++it)
			it->second->clear();

		Listeners.clear();
	}
}

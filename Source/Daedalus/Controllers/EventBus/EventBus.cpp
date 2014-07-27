#include "Daedalus.h"
#include "EventBus.h"

#include <algorithm>

namespace events {
	using ListenerList = EventBus::ListenerList;
	using ListenerMap = EventBus::ListenerMap;

	EventBus::EventBus() {
		Listeners.clear();
	}

	EventBus::~EventBus() {
		for (auto it = Listeners.begin(); it != Listeners.end(); ++it)
			it->second->clear();

		Listeners.clear();
	}

	void EventBus::AddListener(const EventType type, EventListener * const listener) {
		// Insert a list if no listeners are currently listening on this event
		if (Listeners.count(type) == 0) {
			Listeners.insert({
				type,
				std::shared_ptr<ListenerList>(new ListenerList())
			});
		}

		std::shared_ptr<ListenerList> & listeners = Listeners.at(type);

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

	void EventBus::RemoveListener(const EventType type, EventListener * const listener) {
		if (Listeners.count(type) > 0) {
			auto & listeners = Listeners.at(type);

			std::remove_if(
				listeners->begin(),
				listeners->end(),
				[&] (EventListener *& x) { return x == listener; });
		}
	}

	Uint64 EventBus::Count(const EventType type) const {
		if (Listeners.count(type) > 0)
			return Listeners.at(type)->size();
		return 0;
	}

	Uint32 EventBus::BroadcastEvent(const EventDataPtr & data) {
		Uint32 broadcastCount = 0;

		if (Listeners.count(data->Type) > 0) {
			auto & listeners = Listeners.at(data->Type);

			for (auto it = listeners->cbegin(); it != listeners->cend();) {
				(*it)->HandleEvent(data);
				++broadcastCount;
				++it;
			}
		}

		return broadcastCount;
	}
}

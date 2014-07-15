#pragma once

#include "Events.h"
#include "EventListener.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

namespace events {
	using ListenerList = std::vector<EventListener *>;
	using ListenerMap = std::unordered_map<events::EventType, std::shared_ptr<ListenerList>>;

	/**
	*
	*/
	class EventBus {
	private:
		ListenerMap Listeners;
	public:
		EventBus();
		/**
		 * Clean up all pointers stored by this class so garbage collection can do its
		 * work properly.
		 */
		~EventBus();

		/**
		 * Adds an event listener for a particular event. The listener can't listen twice
		 * for the same event. If the listener goes out of scope and the pointer is no
		 * longer valid, then it is automatically removed at some point.
		 */
		void AddListener(
			const EventType type,
			EventListener * const listener);

		void RemoveListener(
			const EventType type,
			EventListener * const listener);

		Uint64 Count(const EventType type) const;

		/**
		 * Broadcast the event along with its event data to all listening interfaces.
		 */
		Uint32 BroadcastEvent(const EventType type, const std::shared_ptr<EventData> & data);
	};

	using EventBusPtr = std::shared_ptr<EventBus>;
}

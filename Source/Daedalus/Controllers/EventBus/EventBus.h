#pragma once

#include "Events.h"
#include "EventListener.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

namespace events {
	typedef std::vector<EventListener *> ListenerList;
	typedef std::unordered_map <events::EventType, std::shared_ptr<ListenerList> > ListenerMap;

	/**
	*
	*/
	class EventBus {
	private:
		events::ListenerMap Listeners;
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
			const events::EventType type,
			EventListener * const listener);

		void RemoveListener(
			const events::EventType type,
			EventListener * const listener);

		uint32_t Count(const events::EventType type) const;

		/**
		 * Broadcast the event along with its event data to all listening interfaces.
		 */
		uint32_t BroadcastEvent(
			const events::EventType type,
			const std::shared_ptr<events::EventData> & data);
	};
}

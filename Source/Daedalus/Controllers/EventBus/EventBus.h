#pragma once

#include "Engine.h"
#include "Events.h"
#include "EventListener.h"

#include <functional>
#include <unordered_map>
#include <vector>

namespace events {
	typedef std::vector<IEventListener *> ListenerList;
	typedef std::unordered_map <events::EventType, TSharedRef<ListenerList> > ListenerMap;

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
			IEventListener * const listener);

		void RemoveListener(
			const events::EventType type,
			IEventListener * const listener);

		uint32 Count(const events::EventType type) const;

		/**
		 * Broadcast the event along with its event data to all listening interfaces.
		 */
		uint32 BroadcastEvent(
			const events::EventType type,
			const TSharedRef<events::EventData> & data);
	};
}

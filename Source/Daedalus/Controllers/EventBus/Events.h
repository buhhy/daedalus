#pragma once

#include <functional>
#include "GameFramework/Character.h"

namespace events {
	enum EventType {
		E_PlayerMovement
	};

	struct EventData {
		EventData() {}
	};

	struct EPlayerMovement : public EventData {
		const TWeakObjectPtr<ACharacter> & Source;
		EPlayerMovement(const TWeakObjectPtr<ACharacter> & source) : Source(source) {}
	};
}

namespace std {
    template <> struct hash<events::EventType> {
        size_t operator()(const events::EventType & tp) const {
            return std::hash<int>()(tp);
        }
    };
}

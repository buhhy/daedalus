#pragma once

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

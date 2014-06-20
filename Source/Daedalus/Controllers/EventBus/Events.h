#pragma once

#include "GameFramework/Character.h"
#include "TerrainDataStructures.h"

#include <functional>
#include <vector>

namespace events {
	enum EventType {
		E_PlayerMovement,
		E_BiomeRegionUpdate
	};

	struct EventData {
		EventData() {}
	};

	struct EPlayerMovement : public EventData {
		const TWeakObjectPtr<ACharacter> & Source;
		EPlayerMovement(const TWeakObjectPtr<ACharacter> & source) : Source(source) {}
	};

	struct EBiomeRegionUpdate : public EventData {
		std::vector<terrain::BiomeOffsetVector> UpdatedOffsets;
		EBiomeRegionUpdate() {}
		EBiomeRegionUpdate(const std::vector<terrain::BiomeOffsetVector> & offs) :
			UpdatedOffsets(offs) {}
	};
}

namespace std {
	template <> struct hash<events::EventType> {
		size_t operator()(const events::EventType & tp) const {
			return std::hash<long>()(tp);
		}
	};
}

#pragma once

#include <Models/Terrain/TerrainDataStructures.h>

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
		const utils::Vector3<> Position;
		EPlayerMovement(const utils::Vector3<> & position) : Position(position) {}
	};

	struct EBiomeRegionUpdate : public EventData {
		std::vector<terrain::BiomeRegionOffsetVector> UpdatedOffsets;
		EBiomeRegionUpdate() {}
		EBiomeRegionUpdate(const std::vector<terrain::BiomeRegionOffsetVector> & offs) :
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

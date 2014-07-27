#pragma once

#include <Models/Terrain/TerrainDataStructures.h>

#include <functional>
#include <vector>

namespace events {
	enum EventType {
		E_PlayerPosition,
		E_ViewPosition,
		E_BiomeRegionUpdate
	};

	struct EventData {
		const EventType Type;

		EventData(const EventType type) : Type(type) {}
	};

	struct EPlayerPosition : public EventData {
		const utils::Point3D Position;

		EPlayerPosition(const utils::Point3D & position) :
			EventData(E_PlayerPosition), Position(position)
		{}
	};

	struct EViewPosition : public EventData {
		const utils::Point3D ViewOrigin;
		const utils::Vector3D<double> ViewDirection;

		EViewPosition(
			const utils::Point3D & origin,
			const utils::Vector3D<double> & viewDir
		) : EventData(E_ViewPosition), ViewOrigin(origin), ViewDirection(viewDir)
		{}
	};

	struct EBiomeRegionUpdate : public EventData {
		std::vector<terrain::BiomeRegionOffsetVector> UpdatedOffsets;

		EBiomeRegionUpdate() : EventData(E_BiomeRegionUpdate) {}
		EBiomeRegionUpdate(const std::vector<terrain::BiomeRegionOffsetVector> & offs) :
			EventData(E_BiomeRegionUpdate), UpdatedOffsets(offs)
		{}
	};

	using EventDataPtr = std::shared_ptr<EventData>;
}

namespace std {
	template <> struct hash<events::EventType> {
		size_t operator()(const events::EventType & tp) const {
			return std::hash<long>()(tp);
		}
	};
}

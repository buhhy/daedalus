#pragma once

#include <Models/Terrain/TerrainDataStructures.h>

#include <functional>
#include <vector>

namespace events {
	enum EventType {
		E_PlayerPosition,
		E_ViewPosition,
		E_FPItemPlacementBegin,
		E_FPItemPlacementEnd,
		E_FPItemPlacementRotation,
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

	// Events related to item placement from the first person perspective.
	struct EFPItemPlacementBegin : public EventData {
		// TODO: probably need some form of player identification here
		EFPItemPlacementBegin() : EventData(E_FPItemPlacementBegin) {}
	};

	struct EFPItemPlacementEnd : public EventData {
		// TODO: probably need some form of player identification here
		const bool bIsCancelled;
		EFPItemPlacementEnd(const bool isCancelled = false) :
			EventData(E_FPItemPlacementEnd), bIsCancelled(isCancelled)
		{}
	};

	struct EFPItemPlacementRotation : public EventData {
		// TODO: probably need some form of player identification here
		const utils::Point2D RotationOffset;
		EFPItemPlacementRotation(const utils::Point2D & mouseOffset) :
			EventData(E_FPItemPlacementRotation), RotationOffset(mouseOffset)
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

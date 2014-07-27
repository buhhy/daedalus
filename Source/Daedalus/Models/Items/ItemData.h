#pragma once

#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <memory>

namespace items {
	enum ItemType {
		I_Chest
	};

	using ItemDataId = std::pair<Uint64, terrain::ChunkPositionVector>;

	/**
	 * This data structure contains item-specific data. An item is some entity placed in the world
	 * that can be interacted with by players or AI. Items in the inventory should reference
	 * an item data structure.
	 */
	struct ItemData {
		ItemDataId ItemId;
		/*
		 Size in grid cells, 1.0 is 1 grid cell, 0.5 is half a grid cell
		 */
		utils::Vector3D<> Size;
		ItemType Type;
		bool bIsPlaced;

		ItemData(
			const ItemType type,
			const ItemDataId & itemId
		) : Type(type),
			ItemId(itemId)
		{}
	};

	using ItemDataPtr = std::shared_ptr<ItemData>;
}

#pragma once

#include <Models/Items/ItemData.h>

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>
#include <unordered_map>

namespace fauna {
	enum CharacterType {
		C_Hero
	};

	struct InventoryItem {
		Uint32 Count;
		items::ItemDataPtr ItemData;

		InventoryItem(const items::ItemDataPtr & data, const Uint32 count) :
			Count(count), ItemData(data)
		{}
	};

	using InventoryItemPtr = std::shared_ptr<InventoryItem>;

	class Inventory {
	public:
		using InventoryItemVector = std::vector<InventoryItemPtr>;

	private:
		// TODO: handle stacking of stateful items, since certain items of the same type
		// could have differing states, and shouldn't stack
		InventoryItemVector Items;
		Uint32 MaxSize;



		InventoryItemVector Filter(const items::ItemType type) {
			InventoryItemVector ret;
			std::copy_if(
				Items.begin(), Items.end(), std::back_inserter(ret),
				[&type] (const InventoryItemPtr & x) {
					return x->ItemData->Template.Type == type;
				});
			return ret;
		}

		const InventoryItemVector Filter(const items::ItemType type) const {
			return Filter(type);
		}

		inline void AssertValidInventory() const {
			assert(Items.size() <= MaxSize &&
				"Inventory::AssertValidInventory: Invalid inventory state");
		}

		Uint64 CountFreeStackSpace(const InventoryItemVector & items) const {
			Uint64 totalAllowableStackCount = 0;
			Uint64 totalItemCount = 0;

			for (const auto & iItem : items) {
				totalItemCount += iItem->Count;
				totalAllowableStackCount += iItem->ItemData->Template.MaxStackSize;
			}

			return totalAllowableStackCount - totalItemCount;
		}

	public:
		Inventory(const Uint32 size) : MaxSize(size) {}

		Uint32 GetMaxSize() const { return MaxSize; }
		const InventoryItemVector & GetItems() const { return Items; }

		bool CanAddItem(const items::ItemDataPtr & item, const Uint32 count) const;
		bool AddItems(const items::ItemDataPtr & item, const Uint32 count = 0);
		bool RemoveItems(const items::ItemDataPtr & item, const Uint32 count = 0);
	};

	using InventoryPtr = std::shared_ptr<Inventory>;

	/**
	 * This template data structure specifies the default parameters and state of the character
	 * data structure.
	 */
	struct CharacterDataTemplate {
		CharacterType Type;
		Uint32 DefaultMaxInventorySize;

		CharacterDataTemplate(const CharacterType type, const Uint32 maxInvSize) :
			Type(type), DefaultMaxInventorySize(maxInvSize)
		{}
	};

	enum EHandAction {
		H_None,
		H_Tool,
		H_Item
	};

	class CharacterData {
	private:
		Uint64 CurrentHeldItemIndex;
		EHandAction CurrentHandAction;

	public:
		Uint64 CharId;
		InventoryPtr InventoryRef;

		const CharacterDataTemplate & Template;



		CharacterData(const Uint64 cid, const CharacterDataTemplate & tmp) :
			CharId(cid), Template(tmp),
			CurrentHeldItemIndex(0), CurrentHandAction(H_None),
			InventoryRef(new Inventory(tmp.DefaultMaxInventorySize))
		{}

		CharacterData(const CharacterDataTemplate & tmp) :
			CharId(0), Template(tmp),
			CurrentHeldItemIndex(0), CurrentHandAction(H_None),
			InventoryRef(new Inventory(tmp.DefaultMaxInventorySize))
		{}

		const Uint64 GetCurrentHeldItemIndex() const { return CurrentHeldItemIndex; }
		const EHandAction GetCurrentHandAction() const { return CurrentHandAction; }

		void NextHeldItem() {
		}

		void PrevHeldItem() {
		}

		void SwitchHandAction(const CurrentHandAction action) {
		}
	};

	using CharacterDataPtr = std::shared_ptr<CharacterData>;
}

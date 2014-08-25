#pragma once

#include <Models/Items/ItemData.h>
#include <Utilities/DataStructures.h>

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>
#include <unordered_map>

namespace fauna {
	enum CharacterType {
		C_Hero
	};

	class InventorySlot {
	private:
		// TODO: handle storing items with differing state
		Uint32 Count;
		items::ItemDataPtr ItemData;

	public:
		InventorySlot() : Count(0), ItemData(NULL) {}

		InventorySlot(const items::ItemDataPtr & data, const Uint32 count) :
			Count(count), ItemData(data)
		{}

		Uint32 GetCount() const { return Count; }
		const items::ItemDataPtr GetItemData() const { return ItemData; }
		bool ContainsItems() const { return Count > 0; }

		void SetItems(const items::ItemDataPtr & item, const Uint32 count) {
			Count = count;
			ItemData = item;
		}

		bool AddItems(const Uint32 count) {
			if (!ContainsItems() || Count + count >= ItemData->Template.MaxStackSize)
				return false;

			Count += count;

			return true;
		}

		bool RemoveItems(const Uint32 count) {
			if (!ContainsItems() || Count < count)
				return false;

			Count -= count;

			if (Count == 0)
				ItemData = NULL;

			return true;
		}
	};

	using InventorySlotPtr = std::shared_ptr<InventorySlot>;

	class Inventory {
	public:
		using InventoryItemVector = std::vector<InventorySlotPtr>;

	private:
		// TODO: handle stacking of stateful items, since certain items of the same type
		// could have differing states, and shouldn't stack
		InventoryItemVector Items;
		Uint32 MaxSize;


		
		inline void AssertValidInventory() const {
			assert(Items.size() <= MaxSize &&
				"Inventory::AssertValidInventory: Invalid inventory state");
		}

		InventoryItemVector Filter(const items::ItemType type) const;
		Uint64 CountFreeStackSpace(const InventoryItemVector & items) const;

	public:
		explicit Inventory(const Uint32 size);
		
		utils::Option<Uint32> GetNextFreeSlot() const;
		Uint32 GetCurrentSize() const;
		Uint32 GetMaxSize() const { return MaxSize; }
		const InventoryItemVector & GetItems() const { return Items; }

		bool CanAddItem(const items::ItemDataPtr & item, const Uint32 count) const;
		bool AddItems(const items::ItemDataPtr & item, const Uint32 count = 1);
		bool RemoveItems(const items::ItemDataPtr & item, const Uint32 count = 1);
		bool RemoveItems(const Uint32 index, const Uint32 count = 1);

		InventorySlotPtr operator [] (const Uint32 index) {
			if (index >= Items.size())
				return NULL;
			return Items[index];
		}
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
		Uint32 CurrentHeldItemIndex;
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

		const Uint32 GetCurrentHeldItemIndex() const { return CurrentHeldItemIndex; }
		const EHandAction GetCurrentHandAction() const { return CurrentHandAction; }

		Uint32 NextHeldItem() {
			CurrentHeldItemIndex++;
			if (CurrentHeldItemIndex >= InventoryRef->GetMaxSize())
				CurrentHeldItemIndex = 0;
			return CurrentHeldItemIndex;
		}

		Uint32 PrevHeldItem() {
			if (CurrentHeldItemIndex == 0)
				CurrentHeldItemIndex = InventoryRef->GetMaxSize();
			CurrentHeldItemIndex--;
			return CurrentHeldItemIndex;
		}

		void SwitchHandAction(const EHandAction action) {
			CurrentHandAction = action;
		}

		InventorySlotPtr GetItemInInventory(const Uint32 index) {
			return (*InventoryRef)[index];
		}

		items::ItemDataPtr GetCurrentItemInInventory() {
			const auto curItem = GetItemInInventory(GetCurrentHeldItemIndex());
			if (!curItem->ContainsItems())
				return NULL;
			return curItem->GetItemData();
		}

		items::ItemDataPtr PlaceCurrentItemInInventory() {
			const auto curIndex = GetCurrentHeldItemIndex();
			const auto curItem = GetItemInInventory(curIndex);

			if (!curItem->ContainsItems())
				return NULL;

			auto itemData = curItem->GetItemData();
			InventoryRef->RemoveItems(curIndex, 1);
			return itemData;
		}
	};

	using CharacterDataPtr = std::shared_ptr<CharacterData>;
}

namespace std {
	template <> struct hash<fauna::CharacterType> {
		size_t operator()(const fauna::CharacterType & tp) const {
			return std::hash<long>()(tp);
		}
	};
}

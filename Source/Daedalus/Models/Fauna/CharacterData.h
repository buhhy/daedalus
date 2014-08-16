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

		bool CanAddItem(const items::ItemDataPtr & item, const Uint32 count) const {
			AssertValidInventory();

			const Uint64 freeSlotSpace = (MaxSize - Items.size()) * item->Template.MaxStackSize;
			const Uint64 freeStackableSpace = CountFreeStackSpace(Filter(item->Template.Type));

			// If all the remaining free slots and free stack space is larger than the number
			// of items we are checking against, then we can insert at least that many items.
			return count <= freeStackableSpace + freeSlotSpace;
		}

		bool AddItems(const items::ItemDataPtr & item, const Uint32 count = 0) {
			AssertValidInventory();

			auto sameTypeItems = Filter(item->Template.Type);

			const auto stackSize = item->Template.MaxStackSize;
			const Uint64 freeSlotSpace = (MaxSize - Items.size()) * stackSize;
			const Uint64 freeStackableSpace = CountFreeStackSpace(sameTypeItems);

			// Ensure we actually have enough space to add items first.
			if (count > freeStackableSpace + freeSlotSpace)
				return false;

			// Fill up the stacked space first.
			Uint32 remainingCount = count;
			for (
				auto it = sameTypeItems.begin();
				remainingCount > 0 && it != sameTypeItems.end();
				++it
			) {
				if ((*it)->Count >= stackSize)
					continue;

				const auto free = stackSize - (*it)->Count;
				const auto take = std::min(free, remainingCount);
				(*it)->Count += take;
				remainingCount -= take;
			}

			// If any more items need to be added, create a new stack.
			if (remainingCount > 0)
				Items.push_back(InventoryItemPtr(new InventoryItem(item, count)));

			return true;
		}

		bool RemoveItems(const items::ItemDataPtr & item, const Uint32 count = 0) {
			AssertValidInventory();
			
			auto sameTypeItems = Filter(item->Template.Type);
			
			Uint64 totalItemCount = 0;

			for (const auto & iItem : sameTypeItems)
				totalItemCount += iItem->Count;

			// Return false if there aren't enough items to remove in one transaction.
			if (totalItemCount < count)
				return false;

			// Sort the stacks from smallest to largest.
			std::sort(
				sameTypeItems.begin(), sameTypeItems.end(),
				[] (const InventoryItemPtr & i1, const InventoryItemPtr & i2) {
					return i1->Count < i2->Count;
				});

			// Remove items from the smallest stacks first.
			Uint32 remainingCount = count;
			for (auto it = sameTypeItems.begin(); remainingCount > 0; ++it) {
				const auto remove = std::min((*it)->Count, remainingCount);
				(*it)->Count += remove;
				remainingCount -= remove;
			}

			// Remove empty stacks from the inventory.
			std::remove_if(
				Items.begin(), Items.end(),
				[] (const InventoryItemPtr & iItem) {
					return iItem->Count == 0;
				});

			return true;
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

	struct CharacterData {
		Uint64 CharId;
		InventoryPtr InventoryRef;

		const CharacterDataTemplate & Template;

		CharacterData(const Uint64 cid, const CharacterDataTemplate & tmp) :
			CharId(cid), Template(tmp), InventoryRef(new Inventory(tmp.DefaultMaxInventorySize))
		{}

		CharacterData(const CharacterDataTemplate & tmp) :
			CharId(0), Template(tmp), InventoryRef(new Inventory(tmp.DefaultMaxInventorySize))
		{}
	};

	using CharacterDataPtr = std::shared_ptr<CharacterData>;
}

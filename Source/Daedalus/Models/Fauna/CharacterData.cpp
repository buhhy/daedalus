#include <daedalus.h>
#include "CharacterData.h"

using namespace utils;

namespace fauna {
	Inventory::Inventory(const Uint32 size) : MaxSize(size) {
		Items.resize(size);
	}

	void Inventory::ClearInvalidItemSlots() {
		for (Uint32 i = 0; i < Items.size(); i++) {
			if (Items[i]->Count == 0)
				Items[i] = NULL;
		}
	}

	bool Inventory::CanAddItem(const items::ItemDataPtr & item, const Uint32 count) const {
		AssertValidInventory();

		const Uint64 freeSlotSpace = (MaxSize - GetCurrentSize()) * item->Template.MaxStackSize;
		const Uint64 freeStackableSpace = CountFreeStackSpace(Filter(item->Template.Type));

		// If all the remaining free slots and free stack space is larger than the number
		// of items we are checking against, then we can insert at least that many items.
		return count <= freeStackableSpace + freeSlotSpace;
	}

	bool Inventory::AddItems(const items::ItemDataPtr & item, const Uint32 count) {
		AssertValidInventory();

		auto sameTypeItems = Filter(item->Template.Type);

		const auto stackSize = item->Template.MaxStackSize;
		const Uint64 freeSlotSpace = (MaxSize - GetCurrentSize()) * stackSize;
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
		while (remainingCount > 0) {
			const auto newSlotIndex = GetNextFreeSlot();
			const auto amount = std::min(item->Template.MaxStackSize, remainingCount);
			if (!newSlotIndex.IsValid())
				break;
			Items[*newSlotIndex] = InventoryItemPtr(new InventoryItem(item, amount));
			remainingCount -= amount;
		}

		return true;
	}

	bool Inventory::RemoveItems(const items::ItemDataPtr & item, const Uint32 count) {
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
		ClearInvalidItemSlots();

		return true;
	}
	
	bool Inventory::RemoveItems(const Uint32 index, const Uint32 count) {
		AssertValidInventory();

		auto itemStack = (*this)[index];

		if (!itemStack || itemStack->Count < count)
			return false;

		itemStack->Count -= count;
		if (itemStack->Count == 0)
			Items[index] = NULL;

		return true;
	}
	
	Option<Uint32> Inventory::GetNextFreeSlot() const {
		for (Uint32 i = 0; i < Items.size(); i++) {
			if (!Items[i] || Items[i]->Count == 0)
				return Some(i);
		}
		return None<Uint32>();
	}

	Uint32 Inventory::GetCurrentSize() const {
		Uint32 count = 0;
		for (Uint32 i = 0; i < Items.size(); i++)
			count += Items[i] && Items[i]->Count > 0 ? 1 : 0;
		return count;
	}
}

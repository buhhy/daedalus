#include <daedalus.h>
#include "CharacterData.h"

using namespace utils;
using namespace items;

namespace fauna {
	/********************************************************************************
	 * Inventory
	 ********************************************************************************/

	Inventory::Inventory(const Uint32 size) : MaxSize(size) {
		for (Uint32 i = 0; i < size; i++)
			Items.push_back(InventorySlotPtr(new InventorySlot()));
	}

	Inventory::InventoryItemVector Inventory::Filter(const items::ItemType type) const {
		InventoryItemVector ret;
		std::copy_if(
			Items.begin(), Items.end(), std::back_inserter(ret),
			[&type] (const InventorySlotPtr & x) {
				return x->ContainsItems() && x->GetItemData()->Template.Type == type;
			});
		return ret;
	}

	Uint64 Inventory::CountFreeStackSpace(const InventoryItemVector & items) const {
		Uint64 totalAllowableStackCount = 0;
		Uint64 totalItemCount = 0;

		for (const auto & iItem : items) {
			totalItemCount += iItem->GetCount();
			totalAllowableStackCount += iItem->GetItemData()->Template.MaxStackSize;
		}

		return totalAllowableStackCount - totalItemCount;
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
			if ((*it)->GetCount() >= stackSize)
				continue;

			const auto free = stackSize - (*it)->GetCount();
			const auto take = std::min(free, remainingCount);
			(*it)->AddItems(take);
			remainingCount -= take;
		}

		// If any more items need to be added, create a new stack.
		while (remainingCount > 0) {
			const auto newSlotIndex = GetNextFreeSlot();
			const auto amount = std::min(item->Template.MaxStackSize, remainingCount);
			if (!newSlotIndex.IsValid())
				break;
			Items[*newSlotIndex]->SetItems(item, amount);
			remainingCount -= amount;
		}

		return true;
	}

	bool Inventory::RemoveItems(const items::ItemDataPtr & item, const Uint32 count) {
		AssertValidInventory();
			
		auto sameTypeItems = Filter(item->Template.Type);
			
		Uint64 totalItemCount = 0;

		for (const auto & iItem : sameTypeItems)
			totalItemCount += iItem->GetCount();

		// Return false if there aren't enough items to remove in one transaction.
		if (totalItemCount < count)
			return false;

		// Sort the stacks from smallest to largest.
		std::sort(
			sameTypeItems.begin(), sameTypeItems.end(),
			[] (const InventorySlotPtr & i1, const InventorySlotPtr & i2) {
				return i1->GetCount() < i2->GetCount();
			});

		// Remove items from the smallest stacks first.
		Uint32 remainingCount = count;
		for (auto it = sameTypeItems.begin(); remainingCount > 0; ++it) {
			const auto remove = std::min((*it)->GetCount(), remainingCount);
			(*it)->RemoveItems(remove);
			remainingCount -= remove;
		}

		return true;
	}
	
	bool Inventory::RemoveItems(const Uint32 index, const Uint32 count) {
		AssertValidInventory();
		return (*this)[index]->RemoveItems(count);
	}
	
	Option<Uint32> Inventory::GetNextFreeSlot() const {
		for (Uint32 i = 0; i < Items.size(); i++) {
			if (!Items[i]->ContainsItems())
				return Some(i);
		}
		return None<Uint32>();
	}

	Uint32 Inventory::GetCurrentSize() const {
		Uint32 count = 0;
		for (Uint32 i = 0; i < Items.size(); i++)
			count += Items[i]->ContainsItems() ? 1 : 0;
		return count;
	}


	
	/********************************************************************************
	 * CharacterData
	 ********************************************************************************/

	Uint32 CharacterData::NextHeldItem() {
		CurrentHeldItemIndex++;
		if (CurrentHeldItemIndex >= InventoryRef->GetMaxSize())
			CurrentHeldItemIndex = 0;
		return CurrentHeldItemIndex;
	}

	Uint32 CharacterData::PrevHeldItem() {
		if (CurrentHeldItemIndex == 0)
			CurrentHeldItemIndex = InventoryRef->GetMaxSize();
		CurrentHeldItemIndex--;
		return CurrentHeldItemIndex;
	}

	ItemDataPtr CharacterData::GetCurrentItemInInventory() {
		const auto curItem = GetItemInInventory(GetCurrentHeldItemIndex());
		if (!curItem->ContainsItems())
			return NULL;
		return curItem->GetItemData();
	}

	ItemDataPtr CharacterData::PlaceCurrentItemInInventory() {
		const auto curIndex = GetCurrentHeldItemIndex();
		const auto curItem = GetItemInInventory(curIndex);

		if (!curItem->ContainsItems())
			return NULL;

		auto itemData = curItem->GetItemData();
		InventoryRef->RemoveItems(curIndex, 1);
		return itemData;
	}

	bool CharacterData::startUsingItem(ItemDataPtr & itemData) {
		if (itemData->addUser(CharId)) {
			currentUsingItem = itemData;
			return true;
		}

		return false;
	}

	bool CharacterData::stopUsingItem() {
		if (!currentUsingItem.expired()) {
			if (currentUsingItem.lock()->removeUser(CharId)) {
				currentUsingItem.reset();
				return true;
			}
		}
		return false;
	}

	bool CharacterData::isUsingItem() const {
		return !currentUsingItem.expired();
	}
}

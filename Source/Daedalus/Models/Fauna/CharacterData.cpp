#include <daedalus.h>
#include "CharacterData.h"

using namespace utils;
using namespace items;

namespace fauna {
	/********************************************************************************
	 * InventorySlot
	 ********************************************************************************/

	void InventorySlot::useShortcut(CharacterDataPtr & source) {
		if (ContainsItems())
			ItemData->interactAction(source);
	}

	std::string InventorySlot::getIconName() const {
		if (ContainsItems())
			return ItemData->Template.resourceName;
		return "";
	}

	utils::Option<Uint32> InventorySlot::getQuantity() const {
		return Some(Count);
	}



	/********************************************************************************
	 * Inventory
	 ********************************************************************************/

	Inventory::Inventory(const Uint32 size) : maxSize(size) {
		for (Uint32 i = 0; i < size; i++)
			items.push_back(InventorySlotPtr(new InventorySlot()));
	}

	Inventory::InventoryItemVector Inventory::Filter(const items::ItemType type) const {
		InventoryItemVector ret;
		std::copy_if(
			items.begin(), items.end(), std::back_inserter(ret),
			[&type] (const InventorySlotPtr & x) {
				return x->ContainsItems() && x->getItemData()->Template.itemType == type;
			});
		return ret;
	}

	Uint64 Inventory::CountFreeStackSpace(const InventoryItemVector & items) const {
		Uint64 totalAllowableStackCount = 0;
		Uint64 totalItemCount = 0;

		for (const auto & iItem : items) {
			totalItemCount += iItem->GetCount();
			totalAllowableStackCount += iItem->getItemData()->Template.maxStackSize;
		}

		return totalAllowableStackCount - totalItemCount;
	}

	bool Inventory::CanAddItem(const items::ItemDataPtr & item, const Uint32 count) const {
		AssertValidInventory();

		const Uint64 freeSlotSpace = (maxSize - GetCurrentSize()) * item->Template.maxStackSize;
		const Uint64 freeStackableSpace = CountFreeStackSpace(Filter(item->Template.itemType));

		// If all the remaining free slots and free stack space is larger than the number
		// of items we are checking against, then we can insert at least that many items.
		return count <= freeStackableSpace + freeSlotSpace;
	}

	bool Inventory::AddItems(const items::ItemDataPtr & item, const Uint32 count) {
		AssertValidInventory();

		auto sameTypeItems = Filter(item->Template.itemType);

		const auto stackSize = item->Template.maxStackSize;
		const Uint64 freeSlotSpace = (maxSize - GetCurrentSize()) * stackSize;
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
			const auto amount = std::min(item->Template.maxStackSize, remainingCount);
			if (!newSlotIndex.IsValid())
				break;
			items[*newSlotIndex]->SetItems(item, amount);
			remainingCount -= amount;
		}

		return true;
	}

	bool Inventory::RemoveItems(const items::ItemDataPtr & item, const Uint32 count) {
		AssertValidInventory();
			
		auto sameTypeItems = Filter(item->Template.itemType);
			
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
		for (Uint32 i = 0; i < items.size(); i++) {
			if (!items[i]->ContainsItems())
				return Some(i);
		}
		return None<Uint32>();
	}

	Uint32 Inventory::GetCurrentSize() const {
		Uint32 count = 0;
		for (Uint32 i = 0; i < items.size(); i++)
			count += items[i]->ContainsItems() ? 1 : 0;
		return count;
	}



	/********************************************************************************
	 * ShortcutBar
	 ********************************************************************************/

	ShortcutBar::ShortcutBar(const Uint32 size) : maxSize(size) {
		shortcuts.resize(size);
	}

	void ShortcutBar::addShortcut(const ShortcutPtr & shortcut, const Uint32 index) {
		if (index < shortcuts.size())
			shortcuts[index] = shortcut;
	}

	void ShortcutBar::removeShortcut(const ShortcutPtr & shortcut, const Uint32 index) {
		if (index < shortcuts.size())
			shortcuts[index] = NULL;
	}
	
	Uint32 ShortcutBar::getMaxSize() const {
		return maxSize;
	}

	ShortcutBar::ShortcutVector::const_iterator ShortcutBar::startIterator() const {
		return shortcuts.cbegin();
	}

	ShortcutBar::ShortcutVector::const_iterator ShortcutBar::endIterator() const {
		return shortcuts.cend();
	}



	/********************************************************************************
	 * CharacterData
	 ********************************************************************************/

	CharacterData::CharacterData(const CharacterDataTemplate & tmp, const Uint64 cid) :
		charId(cid), Template(tmp),
		CurrentHeldItemIndex(0), CurrentHandAction(H_None),
		inventoryRef(new Inventory(tmp.defaultMaxInventorySize)),
		currentShortcutBarRef(new ShortcutBar(tmp.defaultShortcutBarSize)),
		currentHP(tmp.startingMaxHP),
		currentFullness(tmp.startingMaxFullness),
		defaultMaxHP(tmp.startingMaxHP),
		defaultMaxFullness(tmp.startingMaxFullness),
		currentUsingItem()
	{}

	CharacterData::CharacterData(const CharacterDataTemplate & tmp) :
		CharacterData(tmp, 0)
	{}

	Uint32 CharacterData::nextHeldItem() {
		CurrentHeldItemIndex++;
		if (CurrentHeldItemIndex >= inventoryRef->GetMaxSize())
			CurrentHeldItemIndex = 0;
		return CurrentHeldItemIndex;
	}

	Uint32 CharacterData::prevHeldItem() {
		if (CurrentHeldItemIndex == 0)
			CurrentHeldItemIndex = inventoryRef->GetMaxSize();
		CurrentHeldItemIndex--;
		return CurrentHeldItemIndex;
	}

	ItemDataPtr CharacterData::getCurrentItemInInventory() {
		const auto curItem = getItemInInventory(getCurrentHeldItemIndex());
		if (!curItem->ContainsItems())
			return NULL;
		return curItem->getItemData();
	}

	ItemDataPtr CharacterData::placeCurrentItemInInventory() {
		const auto curIndex = getCurrentHeldItemIndex();
		const auto curItem = getItemInInventory(curIndex);

		if (!curItem->ContainsItems())
			return NULL;

		auto itemData = curItem->getItemData();
		inventoryRef->RemoveItems(curIndex, 1);
		return itemData;
	}

	bool CharacterData::startUsingItem(ItemDataPtr & itemData) {
		if (!currentUsingItem.expired()) {
			// If the player is currently using this item already, don't start using it.
			if (itemData->getItemId() == currentUsingItem.lock()->getItemId())
				return false;

			// Stop using any current items before using the current one.
			stopUsingItem();
		}

		if (itemData->addUser(charId)) {
			currentUsingItem = itemData;
			return true;
		}

		return false;
	}

	bool CharacterData::stopUsingItem() {
		if (!currentUsingItem.expired()) {
			if (currentUsingItem.lock()->removeUser(charId)) {
				currentUsingItem.reset();
				return true;
			}
		}
		return false;
	}

	bool CharacterData::isUsingItem() const {
		return !currentUsingItem.expired();
	}

	void CharacterData::addToCurrentShortcutSet(
		const ShortcutPtr & shortcut,
		const Uint32 index
	) {
		currentShortcutBarRef->addShortcut(shortcut, index);
	}

	void CharacterData::removeFromCurrentShortcutSet(
		const ShortcutPtr & shortcut,
		const Uint32 index
	) {
		currentShortcutBarRef->removeShortcut(shortcut, index);
	}

	InventoryCPtr CharacterData::getInventory() const {
		return inventoryRef;
	}

	ShortcutBarCPtr CharacterData::getCurrentShortcutSet() const {
		return currentShortcutBarRef;
	}
}

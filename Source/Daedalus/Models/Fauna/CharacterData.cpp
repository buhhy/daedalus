#include <daedalus.h>
#include "CharacterData.h"

using namespace utils;
using namespace items;

namespace fauna {

	/********************************************************************************
	 * InventorySlot
	 ********************************************************************************/

	void InventorySlot::quickuse(const CharacterDataPtr & source) {
		if (containsItems())
			ItemData->interactAction(source);
	}

	std::string InventorySlot::getIconName() const {
		if (containsItems())
			return ItemData->Template.resourceName;
		return "";
	}

	utils::Option<Uint32> InventorySlot::getQuantity() const {
		return Some(Count);
	}

	bool InventorySlot::isValid() const {
		return containsItems();
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
				return x->containsItems() && x->getItemData()->Template.itemType == type;
			});
		return ret;
	}

	Uint64 Inventory::CountFreeStackSpace(const InventoryItemVector & items) const {
		Uint64 totalAllowableStackCount = 0;
		Uint64 totalItemCount = 0;

		for (const auto & iItem : items) {
			totalItemCount += iItem->getCount();
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
			if ((*it)->getCount() >= stackSize)
				continue;

			const auto free = stackSize - (*it)->getCount();
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
			totalItemCount += iItem->getCount();

		// Return false if there aren't enough items to remove in one transaction.
		if (totalItemCount < count)
			return false;

		// Sort the stacks from smallest to largest.
		std::sort(
			sameTypeItems.begin(), sameTypeItems.end(),
			[] (const InventorySlotPtr & i1, const InventorySlotPtr & i2) {
				return i1->getCount() < i2->getCount();
			});

		// Remove items from the smallest stacks first.
		Uint32 remainingCount = count;
		for (auto it = sameTypeItems.begin(); remainingCount > 0; ++it) {
			const auto remove = std::min((*it)->getCount(), remainingCount);
			(*it)->RemoveItems(remove);
			remainingCount -= remove;
		}

		return true;
	}
	
	bool Inventory::RemoveItems(const Uint32 index, const Uint32 count) {
		AssertValidInventory();
		// TODO: this doesn't work
		return (*this)[index]->RemoveItems(count);
	}

	void Inventory::swapItems(const Uint32 index1, const Uint32 index2) {
		if (index1 >= maxSize || index2 >= maxSize)
			return;

		const auto swap = items[index1];
		items[index1] = items[index2];
		items[index2] = swap;
	}
	
	Option<Uint32> Inventory::GetNextFreeSlot() const {
		for (Uint32 i = 0; i < items.size(); i++) {
			if (!items[i]->containsItems())
				return Some(i);
		}
		return None<Uint32>();
	}

	Uint32 Inventory::GetCurrentSize() const {
		Uint32 count = 0;
		for (Uint32 i = 0; i < items.size(); i++)
			count += items[i]->containsItems() ? 1 : 0;
		return count;
	}



	/********************************************************************************
	 * QuickuseBar
	 ********************************************************************************/

	QuickuseBar::QuickuseBar(const Uint32 size) : maxSize(size) {
		shortcuts.resize(size);
	}

	void QuickuseBar::addShortcut(const IQuickusePtr & shortcut, const Uint32 index) {
		if (index < shortcuts.size())
			shortcuts[index] = shortcut;
	}

	void QuickuseBar::removeShortcut(const Uint32 index) {
		if (index < shortcuts.size())
			shortcuts[index] = NULL;
	}
	
	void QuickuseBar::quickuse(const CharacterDataPtr & charData, const Uint32 index) {
		auto shortcut = (*this)[index];
		if (shortcut)
			shortcut->quickuse(charData);
	}
	
	Uint32 QuickuseBar::getMaxSize() const {
		return maxSize;
	}

	QuickuseBar::QuickuseVector::const_iterator QuickuseBar::startIterator() const {
		return shortcuts.cbegin();
	}

	QuickuseBar::QuickuseVector::const_iterator QuickuseBar::endIterator() const {
		return shortcuts.cend();
	}

	void QuickuseBar::cullShortcuts() {
		for (Uint32 i = 0; i < shortcuts.size(); i++) {
			if (shortcuts[i] && !shortcuts[i]->isValid())
				shortcuts[i] = nullptr;
		}
	}



	/********************************************************************************
	 * CharacterData
	 ********************************************************************************/

	CharacterData::CharacterData(const CharacterDataTemplate & tmp, const Uint64 cid) :
		charId(cid), Template(tmp),
		CurrentHeldItemIndex(0), CurrentHandAction(H_None),
		inventoryRef(new Inventory(tmp.defaultMaxInventorySize)),
		currentShortcutBarRef(new QuickuseBar(tmp.defaultShortcutBarSize)),
		currentHP(tmp.startingMaxHP),
		currentFullness(tmp.startingMaxFullness),
		defaultMaxHP(tmp.startingMaxHP),
		defaultMaxFullness(tmp.startingMaxFullness),
		currentUsingItem()
	{}

	CharacterData::CharacterData(const CharacterDataTemplate & tmp) :
		CharacterData(tmp, 0)
	{}


	/****************************************
	 * Inventory management
	 ****************************************/

	void CharacterData::switchHeldItem(const Uint32 index) {
	}

	void CharacterData::switchHandAction(const EHandAction action) {
		CurrentHandAction = action;
	}

	ItemDataPtr CharacterData::getCurrentItemInInventory() {
		const auto curItem = getItemInInventory(getCurrentHeldItemIndex());
		if (!curItem->containsItems())
			return NULL;
		return curItem->getItemData();
	}

	ItemDataPtr CharacterData::placeCurrentItemInInventory() {
		const auto curIndex = getCurrentHeldItemIndex();
		const auto curItem = getItemInInventory(curIndex);

		if (!curItem->containsItems())
			return NULL;

		auto itemData = curItem->getItemData();
		inventoryRef->RemoveItems(curIndex, 1);
		currentShortcutBarRef->cullShortcuts();
		return itemData;
	}

	InventorySlotPtr CharacterData::getItemInInventory(const Uint32 index) {
		return (*inventoryRef)[index];
	}

	bool CharacterData::addItemsToInventory(const ItemDataPtr & item, const Uint32 count) {
		return inventoryRef->AddItems(item, count);
	}

	InventoryCPtr CharacterData::getInventory() const {
		return inventoryRef;
	}

	InventoryPtr CharacterData::getInventory() {
		return inventoryRef;
	}

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


	/****************************************
	 * Quickuse bar
	 ****************************************/

	QuickuseBarCPtr CharacterData::getCurrentShortcutSet() const {
		return currentShortcutBarRef;
	}

	QuickuseBarPtr CharacterData::getCurrentShortcutSet() {
		return currentShortcutBarRef;
	}

	void CharacterData::quickuse(const Uint32 index) {
		currentShortcutBarRef->quickuse(shared_from_this(), index);
	}

	void CharacterData::addToCurrentShortcutSet(
		const IQuickusePtr & shortcut,
		const Uint32 index
	) {
		currentShortcutBarRef->addShortcut(shortcut, index);
	}

	void CharacterData::removeFromCurrentShortcutSet(const Uint32 index) {
		currentShortcutBarRef->removeShortcut(index);
	}


	/****************************************
	 * Environment interactions
	 ****************************************/

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
}

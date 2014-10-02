#pragma once

#include <Models/Shortcut.h>
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

	class InventorySlot : public IShortcut {
	private:
		// TODO: handle storing items with differing state
		Uint32 Count;
		items::ItemDataPtr ItemData;

	public:
		InventorySlot() : Count(0), ItemData(NULL) {}

		InventorySlot(const items::ItemDataPtr & data, const Uint32 count) :
			Count(count), ItemData(data)
		{}
		
		// Override from Shortcut.
		virtual void useShortcut(fauna::CharacterDataPtr & charData) override;
		virtual std::string getIconName() const override;
		virtual utils::Option<Uint32> getQuantity() const override;
		virtual bool isValid() const override;

		Uint32 getCount() const { return Count; }
		const items::ItemDataPtr getItemData() const { return ItemData; }
		bool containsItems() const { return Count > 0; }

		void SetItems(const items::ItemDataPtr & item, const Uint32 count) {
			Count = count;
			ItemData = item;
		}

		bool AddItems(const Uint32 count) {
			if (!containsItems() || Count + count >= ItemData->Template.maxStackSize)
				return false;

			Count += count;

			return true;
		}

		bool RemoveItems(const Uint32 count) {
			if (!containsItems() || Count < count)
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
		InventoryItemVector items;
		Uint32 maxSize;


		
		inline void AssertValidInventory() const {
			assert(Items.size() <= maxSize &&
				"Inventory::AssertValidInventory: Invalid inventory state");
		}

		InventoryItemVector Filter(const items::ItemType type) const;
		Uint64 CountFreeStackSpace(const InventoryItemVector & items) const;

	public:
		explicit Inventory(const Uint32 size);
		
		utils::Option<Uint32> GetNextFreeSlot() const;
		Uint32 GetCurrentSize() const;
		Uint32 GetMaxSize() const { return maxSize; }
		const InventoryItemVector & GetItems() const { return items; }

		bool CanAddItem(const items::ItemDataPtr & item, const Uint32 count) const;
		bool AddItems(const items::ItemDataPtr & item, const Uint32 count = 1);
		bool RemoveItems(const items::ItemDataPtr & item, const Uint32 count = 1);
		bool RemoveItems(const Uint32 index, const Uint32 count = 1);

		InventorySlotPtr operator [] (const Uint32 index) {
			if (index >= items.size())
				return NULL;
			return items[index];
		}
	};

	using InventoryPtr = std::shared_ptr<Inventory>;
	using InventoryCPtr = std::shared_ptr<const Inventory>;

	class ShortcutBar {
	public:
		using ShortcutVector = std::vector<ShortcutPtr>;

	private:
		ShortcutVector shortcuts;
		Uint32 maxSize;

	public:
		explicit ShortcutBar(const Uint32 size);

		ShortcutPtr operator [] (const Uint32 index) {
			if (index >= shortcuts.size())
				return NULL;
			return shortcuts[index];
		}

		void addShortcut(const ShortcutPtr & shortcut, const Uint32 index);
		void removeShortcut(const ShortcutPtr & shortcut, const Uint32 index);
		/**
		 * Removes shortcuts that have become invalid.
		 */
		void cullShortcuts();
		Uint32 getMaxSize() const;
		ShortcutVector::const_iterator startIterator() const;
		ShortcutVector::const_iterator endIterator() const;
	};

	using ShortcutBarPtr = std::shared_ptr<ShortcutBar>;
	using ShortcutBarCPtr = std::shared_ptr<const ShortcutBar>;

	/**
	 * This template data structure specifies the default parameters and state of the character
	 * data structure.
	 */
	struct CharacterDataTemplate {
		CharacterType Type;
		Uint32 defaultMaxInventorySize;
		Uint32 defaultShortcutBarSize;
		Uint32 startingMaxHP;
		Uint32 startingMaxFullness;

		CharacterDataTemplate(
			const CharacterType type,
			const Uint32 maxInvSize,
			const Uint32 shortcutBarSize,
			const Uint32 startingMaxHP,
			const Uint32 startingMaxFullness
		) : Type(type), defaultMaxInventorySize(maxInvSize),
			defaultShortcutBarSize(shortcutBarSize),
			startingMaxHP(startingMaxHP), startingMaxFullness(startingMaxFullness)
		{}
	};

	enum EHandAction {
		H_None,
		H_Tool,
		H_Item
	};

	class CharacterData {
	private:
		Uint64 charId;
		Uint32 CurrentHeldItemIndex;
		EHandAction CurrentHandAction;
		std::weak_ptr<items::ItemData> currentUsingItem;
		Uint32 currentHP;
		Uint32 currentFullness;
		Uint32 defaultMaxHP;
		Uint32 defaultMaxFullness;

		ShortcutBarPtr currentShortcutBarRef;

	public:
		InventoryPtr inventoryRef;

		const CharacterDataTemplate & Template;



		CharacterData(const CharacterDataTemplate & tmp, const Uint64 cid);
		CharacterData(const CharacterDataTemplate & tmp);
		
		Uint64 getCharId() const { return charId; }

		Uint32 getCurrentHeldItemIndex() const { return CurrentHeldItemIndex; }
		EHandAction getCurrentHandAction() const { return CurrentHandAction; }
		items::ItemDataPtr getCurrentItemInInventory();
		items::ItemDataPtr placeCurrentItemInInventory();
		InventorySlotPtr getItemInInventory(const Uint32 index) {
			return (*inventoryRef)[index];
		}

		void addToCurrentShortcutSet(const ShortcutPtr & shortcut, const Uint32 index);
		void removeFromCurrentShortcutSet(const ShortcutPtr & shortcut, const Uint32 index);

		InventoryCPtr getInventory() const;
		ShortcutBarCPtr getCurrentShortcutSet() const;
		ShortcutBarPtr getCurrentShortcutSet();

		Uint32 getCurrentHP() const { return currentHP; }
		Uint32 getCurrentFullness() const { return currentFullness; }
		Uint32 getDefaultMaxHP() const { return defaultMaxHP; }
		Uint32 getDefaultMaxFullness() const { return defaultMaxFullness; }
		
		void setCurrentHP(const Uint32 cHP) { currentHP = cHP; }
		void setCurrentFullness(const Uint32 cFullness) { currentFullness = cFullness; }

		void switchHandAction(const EHandAction action) {
			CurrentHandAction = action;
		}

		Uint32 nextHeldItem();
		Uint32 prevHeldItem();

		bool startUsingItem(items::ItemDataPtr & itemData);
		bool stopUsingItem();
		bool isUsingItem() const;
		const std::weak_ptr<items::ItemData> & getUsingItem() const {
			return currentUsingItem;
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

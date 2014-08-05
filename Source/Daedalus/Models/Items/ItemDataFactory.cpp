#include <Daedalus.h>
#include "ItemDataFactory.h"

namespace items {
	ItemDataFactory::ItemDataFactory() {
		LoadItemDataTemplates();
	}

	void ItemDataFactory::LoadItemDataTemplates() {
		// TODO: load item template data from files
		ItemTemplates.insert(std::make_pair(
			I_Chest,
			ItemDataTemplateUPtr(new ItemDataTemplate(
				I_Chest, ItemRotation(4, 1), { 1.0, 1.0, 1.0 }, { 0.5, 0.5, 0.5 }, "Chest.Chest"))));
	}

	ItemDataPtr ItemDataFactory::BuildItemData(const ItemType type) const {
		const auto found = ItemTemplates.find(type);
		if (found == ItemTemplates.end())
			return NULL;
		return ItemDataPtr(new ItemData(0, { 0, 0 }, ItemRotation(1, 0), false, *found->second));
	}
}

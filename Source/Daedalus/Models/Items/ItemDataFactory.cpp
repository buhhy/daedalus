#include <Daedalus.h>
#include "ItemDataFactory.h"

namespace items {
	using namespace utils;

	ItemDataFactory::ItemDataFactory() {
		LoadItemDataTemplates();
	}

	void ItemDataFactory::LoadItemDataTemplates() {
		// TODO: load item template data from files
		ItemTemplates.insert(std::make_pair(
			I_Chest,
			ItemDataTemplateUPtr(new ItemDataTemplate(
				I_Chest, ItemRotation(4, 1),
				Vector3D<>(1.0, 1.0, 1.0), Point3D(0.5, 0.5, 0.5), "Chest.Chest"))));
		ItemTemplates.insert(std::make_pair(
			I_Sofa,
			ItemDataTemplateUPtr(new ItemDataTemplate(
				I_Chest, ItemRotation(4, 1),
				Vector3D<>(2.0, 1.0, 1.0), Point3D(0.5, 0.5, 0.5), "Sofa.Sofa"))));
	}

	ItemDataPtr ItemDataFactory::BuildItemData(const ItemType type) const {
		const auto found = ItemTemplates.find(type);
		if (found == ItemTemplates.end())
			return NULL;
		return ItemDataPtr(new ItemData(*found->second));
	}
}

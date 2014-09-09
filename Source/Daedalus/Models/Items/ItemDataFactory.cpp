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
				AxisAlignedBoundingBox3D(Point3D(0), Point3D(2.0)),
				Point3D(1, 1, 1), "Chest", 25, 2.0))));
		ItemTemplates.insert(std::make_pair(
			I_Sofa,
			ItemDataTemplateUPtr(new ItemDataTemplate(
				I_Sofa, ItemRotation(4, 1),
				AxisAlignedBoundingBox3D(Point3D(0), Point3D(4.0, 2.0, 2.0)),
				Point3D(1, 1, 1), "Sofa", 25, 2.0))));
	}

	ItemDataPtr ItemDataFactory::BuildItemData(const ItemType type) const {
		const auto found = ItemTemplates.find(type);
		if (found == ItemTemplates.end())
			return NULL;
		return ItemDataPtr(new ItemData(*found->second));
	}
}

#include <Daedalus.h>
#include "ItemDataFactory.h"

namespace items {
	using namespace utils;

	ItemDataFactory::ItemDataFactory() {
		loadItemDataTemplates();
	}

	void ItemDataFactory::loadItemDataTemplates() {
		// TODO: load item template data from files
		itemTemplates.insert(std::make_pair(
			I_Chest,
			ItemDataTemplatePtr(new ItemDataTemplate(
				I_Chest, ItemRotation(4, 1),
				AxisAlignedBoundingBox3D(Point3D(0), Point3D(2.0)),
				Point3D(1, 1, 1), "Chest", 25, 2.0))));
		itemTemplates.insert(std::make_pair(
			I_Sofa,
			ItemDataTemplatePtr(new ItemDataTemplate(
				I_Sofa, ItemRotation(4, 1),
				AxisAlignedBoundingBox3D(Point3D(0), Point3D(4.0, 2.0, 2.0)),
				Point3D(1, 1, 1), "Sofa", 25, 2.0))));
	}

	ItemDataPtr ItemDataFactory::BuildItemData(const ItemType type) const {
		const auto found = itemTemplates.find(type);
		if (found == itemTemplates.end())
			return NULL;
		return ItemDataPtr(new ItemData(*found->second));
	}

	const ItemDataFactory::ItemDataTemplateMap & ItemDataFactory::getItemDataTemplates() const {
		return itemTemplates;
	}
}

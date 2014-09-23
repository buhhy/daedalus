#pragma once

#include <Models/Items/ItemData.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <memory>
#include <unordered_map>

namespace items {
	class ItemDataFactory {
	public:
		using ItemDataTemplateMap = std::unordered_map<ItemType, ItemDataTemplatePtr>;

	private:
		ItemDataTemplateMap itemTemplates;

		void loadItemDataTemplates();

	public:
		ItemDataFactory();

		ItemDataPtr BuildItemData(const ItemType type) const;
		const ItemDataTemplateMap & getItemDataTemplates() const;
	};

	using ItemDataFactoryPtr = std::shared_ptr<ItemDataFactory>;
	using ItemDataFactoryCPtr = std::shared_ptr<const ItemDataFactory>;
}

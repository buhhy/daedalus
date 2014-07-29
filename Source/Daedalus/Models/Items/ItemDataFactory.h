#pragma once

#include <Models/Items/ItemData.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <memory>
#include <unordered_map>

namespace items {
	class ItemDataFactory {
	public:
		using ItemDataTemplateUPtr = std::unique_ptr<ItemDataTemplate>;
		using ItemDataTemplateMap = std::unordered_map<ItemType, ItemDataTemplateUPtr>;

	private:
		ItemDataTemplateMap ItemTemplates;

		void LoadItemDataTemplates();

	public:
		ItemDataFactory();

		ItemDataPtr BuildItemData(const ItemType type) const;

	};

	using ItemDataFactoryPtr = std::shared_ptr<ItemDataFactory>;
}

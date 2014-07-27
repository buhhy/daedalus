#include <Daedalus.h>
#include "ItemFactory.h"

using namespace items;

UItemFactory::UItemFactory(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP)
{
	auto a = AItem::StaticClass();
}

UClass * UItemFactory::GetItemClass(const ItemDataPtr & itemData) const {
	switch (itemData->Type) {
	case I_Chest:
		return AChest::StaticClass();
	default:
		return NULL;
	}
}

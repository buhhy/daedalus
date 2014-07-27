#pragma once

#include <string>
#include <Actors/Items/Chest.h>
#include <Models/Items/ItemData.h>

#include "GameFramework/Actor.h"
#include "ItemFactory.generated.h"

/**
 * 
 */
UCLASS()
class UItemFactory : public UObject {
	GENERATED_UCLASS_BODY()
protected:

public:
	UClass * GetItemClass(const items::ItemDataPtr & itemData) const;
	
};

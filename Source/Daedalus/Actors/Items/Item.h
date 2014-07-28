#pragma once

#include <string>

#include <GameFramework/Actor.h>

#include <Models/Items/ItemData.h>

#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class AItem : public AActor {
	GENERATED_UCLASS_BODY()
protected:
	items::ItemDataPtr ItemData;


	UStaticMesh * FindStaticMesh(const TCHAR * name) const {
		return ConstructorHelpers::FObjectFinder<UStaticMesh>(name).Object;
	}

	UMaterial * FindMaterial(const TCHAR * name) const {
		return ConstructorHelpers::FObjectFinder<UMaterial>(name).Object;
	}

	void PostInitialize();

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Static Mesh")
		TSubobjectPtr<UStaticMeshComponent> MeshComponent;
	
	items::ItemDataPtr & GetItemData() { return ItemData; }
	const items::ItemDataPtr & GetItemData() const { return ItemData; }
	void SetItemData(const items::ItemDataPtr & data);

};

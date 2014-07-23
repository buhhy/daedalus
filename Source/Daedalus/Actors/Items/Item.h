#pragma once

#include <string>

#include "GameFramework/Actor.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class AItem : public AActor {
	GENERATED_UCLASS_BODY()
protected:
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
	
};

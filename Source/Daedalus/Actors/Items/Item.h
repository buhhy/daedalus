#pragma once

#include <string>

#include <GameFramework/Actor.h>

#include <Models/Items/ItemData.h>
#include <Models/Terrain/TerrainDataStructures.h>

#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class AItem : public AActor {
	GENERATED_UCLASS_BODY()
protected:
	items::ItemDataPtr ItemData;
	utils::Matrix4D<> TranslationMatrix;
	utils::Matrix4D<> RotationMatrix;

	const terrain::TerrainGeneratorParameters * GenParams;



	template <typename ObjClass>
	ObjClass * LoadObjFromPath(const TCHAR * path) {
		if(path == NULL) return NULL;
		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, path));
	}

	UStaticMesh * FindStaticMesh(const TCHAR * name) { return LoadObjFromPath<UStaticMesh>(name); }
	UMaterial * FindMaterial(const TCHAR * name) { return LoadObjFromPath<UMaterial>(name); }

	void AssertInitialized() const;
	void LoadMesh(const std::string & meshName);
	void AdjustRotationMatrix();
	void AdjustPositionMatrix();
	void ApplyTransform();

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Static Mesh")
		TSubobjectPtr<UStaticMeshComponent> MeshComponent;
	
	items::ItemDataPtr & GetItemData() { return ItemData; }
	const items::ItemDataPtr & GetItemData() const { return ItemData; }
	void Initialize(const items::ItemDataPtr & data);
	void SetPosition(const terrain::ChunkPositionVector & position);
	void SetRotation(const items::ItemRotation & rotation);
};

#pragma once

#include <string>

#include <GameFramework/Actor.h>

#include <Models/Items/ItemData.h>
#include <Models/Terrain/TerrainDataStructures.h>

#include <string>
#include <unordered_map>

#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class AItem : public AActor {
	GENERATED_UCLASS_BODY()

protected:
	using ItemMeshCache = std::unordered_map<std::string, UStaticMesh *>;
	using ItemMaterialCache = std::unordered_map<std::string, UMaterial *>;

	items::ItemDataPtr ItemData;

	ItemMeshCache MeshCache;
	ItemMaterialCache MaterialCache;
	const terrain::TerrainGeneratorParameters * TerrainParams;



	template <typename ObjClass>
	ObjClass * LoadObjFromPath(const TCHAR * path) {
		if (path == NULL) return NULL;
		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, path));
	}

	UStaticMesh * FindStaticMesh(const std::string & nameStr) {
		const TCHAR * name = *FString(nameStr.c_str());
		auto found = MeshCache.find(nameStr);

		if (found != MeshCache.end())
			return found->second;
		else {
			auto mesh = LoadObjFromPath<UStaticMesh>(name);
			MeshCache.insert({ nameStr, mesh });
			return mesh;
		}
	}

	UMaterial * FindMaterial(const std::string & nameStr) {
		const TCHAR * name = *FString(nameStr.c_str());
		auto found = MaterialCache.find(nameStr);

		if (found != MaterialCache.end())
			return found->second;
		else {
			auto material = LoadObjFromPath<UMaterial>(name);
			MaterialCache.insert({ nameStr, material });
			return material;
		}
	}

	void AssertInitialized() const;
	void LoadMesh(const std::string & meshName);
	void ApplyTransform();

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Static Mesh")
		TSubobjectPtr<UStaticMeshComponent> MeshComponent;
	
	items::ItemDataPtr & GetItemData() { return ItemData; }
	const items::ItemDataPtr & GetItemData() const { return ItemData; }
	void Initialize(const items::ItemDataPtr & data);
	void SetPosition(const terrain::ChunkPositionVector & position);
	void SetRotation(const items::ItemRotation & rotation);

	virtual void BeginPlay() override;
};

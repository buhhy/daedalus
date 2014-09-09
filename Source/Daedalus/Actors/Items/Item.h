#pragma once

#include <string>

#include <GameFramework/Actor.h>

#include <Models/Items/ItemData.h>
#include <Models/Terrain/TerrainDataStructures.h>

#include <string>
#include <unordered_map>

#include "Item.generated.h"

class APlayerCharacter;

/**
 * 
 */
UCLASS()
class AItem : public AActor {
	GENERATED_UCLASS_BODY()
protected:
	using ItemMeshCache = std::unordered_map<std::string, USkeletalMesh *>;
	using AnimBlueprintCache = std::unordered_map<std::string, UAnimBlueprint *>;
	using ItemMaterialCache = std::unordered_map<std::string, UMaterial *>;

	items::ItemDataPtr ItemData;

	ItemMeshCache MeshCache;
	ItemMaterialCache MaterialCache;
	AnimBlueprintCache AnimBPCache;

	items::ItemRotation lastRotation;
	terrain::ChunkPositionVector lastPosition;
	utils::Vector3D<> lastScale;
	float tickCount;

	const terrain::TerrainGeneratorParameters * TerrainParams;



	template <typename ObjClass>
	ObjClass * LoadObjFromPath(const TCHAR * path) {
		// TODO: eventually use global precached meshes instead of dunamic fetching
		if (path == NULL) return NULL;
		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, path));
	}

	USkeletalMesh * FindSkeletalMesh(const std::string & nameStr) {
		const TCHAR * name = UTF8_TO_TCHAR(nameStr.c_str());
		auto found = MeshCache.find(nameStr);

		if (found != MeshCache.end())
			return found->second;
		else {
			auto mesh = LoadObjFromPath<USkeletalMesh>(name);
			MeshCache.insert({ nameStr, mesh });
			return mesh;
		}
	}

	UMaterial * FindMaterial(const std::string & nameStr) {
		const TCHAR * name = UTF8_TO_TCHAR(nameStr.c_str());
		auto found = MaterialCache.find(nameStr);

		if (found != MaterialCache.end())
			return found->second;
		else {
			auto material = LoadObjFromPath<UMaterial>(name);
			MaterialCache.insert({ nameStr, material });
			return material;
		}
	}

	UAnimBlueprint * FindAnimBlueprint(const std::string & nameStr) {
		const TCHAR * name = UTF8_TO_TCHAR(nameStr.c_str());
		auto found = AnimBPCache.find(nameStr);

		if (found != AnimBPCache.end())
			return found->second;
		else {
			auto animBP = LoadObjFromPath<UAnimBlueprint>(name);
			AnimBPCache.insert({ nameStr, animBP });
			return animBP;
		}
	}

	void AssertInitialized() const;
	void LoadMesh(const std::string & meshName);
	void SetRelativeTransform(const FVector & location, const FRotator & rot);
	virtual void applyTransform();
	void applyScale();

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Static Mesh")
		TSubobjectPtr<USkeletalMeshComponent> MeshComponent;

	UFUNCTION(
		BlueprintPure,
		meta = (
			FriendlyName = "Is Item Currently In Use",
			CompactNodeTitle = "In Use",
			Keywords = "use"),
		Category = "Item Properties")
		bool IsItemInUse();

	virtual void BeginPlay() override;
	virtual void Tick(float interval) override;

	items::ItemDataPtr & getItemData() { return ItemData; }
	const items::ItemDataPtr & getItemData() const { return ItemData; }
	virtual void initialize(const items::ItemDataPtr & data);
};

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
	template <typename T>
	using Cache = std::unordered_map<std::string, T *>;

	items::ItemDataPtr ItemData;
	
	Cache<UStaticMesh> staticMeshCache;
	Cache<USkeletalMesh> skeletalMeshCache;
	Cache<UMaterial> materialCache;
	Cache<UAnimBlueprint> animBPCache;

	items::ItemRotation lastRotation;
	terrain::ChunkPositionVector lastPosition;
	utils::Vector3D<> lastScale;
	float tickCount;

	const terrain::TerrainGeneratorParameters * TerrainParams;



	template <typename ObjClass>
	ObjClass * loadObjFromPath(const TCHAR * path) {
		// TODO: eventually use global precached meshes instead of dunamic fetching
		if (path == NULL) return NULL;
		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, path));
	}

	template <typename ObjClass>
	ObjClass * findResource(Cache<ObjClass> & cache, const std::string & nameStr) {
		const TCHAR * name = UTF8_TO_TCHAR(nameStr.c_str());
		auto found = cache.find(nameStr);

		if (found != cache.end())
			return found->second;
		else {
			auto loaded = loadObjFromPath<ObjClass>(name);
			cache.insert({ nameStr, loaded });
			return loaded;
		}
	}

	UStaticMesh * findStaticMesh(const std::string & nameStr) {
		return findResource(staticMeshCache, nameStr);
	}

	USkeletalMesh * findSkeletalMesh(const std::string & nameStr) {
		return findResource(skeletalMeshCache, nameStr);
	}

	UMaterial * findMaterial(const std::string & nameStr) {
		return findResource(materialCache, nameStr);
	}

	UAnimBlueprint * findAnimBlueprint(const std::string & nameStr) {
		return findResource(animBPCache, nameStr);
	}

	void AssertInitialized() const;
	void loadMesh(const std::string & meshName);
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

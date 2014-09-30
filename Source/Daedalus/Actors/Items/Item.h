#pragma once

#include <string>

#include <Actors/DDActor.h>

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
class AItem : public ADDActor {
	GENERATED_UCLASS_BODY()
protected:
	items::ItemDataPtr ItemData;

	items::ItemRotation lastRotation;
	terrain::ChunkPositionVector lastPosition;
	utils::Vector3D<> lastScale;
	float tickCount;

	const terrain::TerrainGeneratorParameters * terrainParams;



	void AssertInitialized() const;
	void loadResources(const std::string & meshName);
	void SetRelativeTransform(const FVector & location, const FRotator & rot);
	virtual void applyTransform();
	void applyScale();

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Resources")
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

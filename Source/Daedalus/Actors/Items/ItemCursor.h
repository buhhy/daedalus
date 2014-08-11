#pragma once

#include <string>

#include <Actors/Items/Item.h>
#include <Models/Items/ItemData.h>
#include <Models/Terrain/TerrainDataStructures.h>

#include "ItemCursor.generated.h"

/**
 * 
 */
UCLASS()
class AItemCursor : public AItem {
	GENERATED_UCLASS_BODY()
protected:
	terrain::ChunkPositionVector PlayerPosition;
	FMatrix PlayerRotation;



	utils::Vector3D<> GetOffsetVector() const;
	virtual void ApplyTransform() override;

public:
	void SetPlayerTransform(const utils::Point3D & position, const FMatrix & rotation);
};

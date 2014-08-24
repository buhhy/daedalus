#pragma once

#include <string>

#include <Actors/Items/Item.h>
#include <Models/Items/ItemData.h>
#include <Models/Terrain/TerrainDataStructures.h>

#include "ItemCursor.generated.h"

/**
 * The item cursor is an extended item object that acts as the ghost cursor for the player when
 * placing items. It snaps to the grid and is placed like a normal item.
 */
UCLASS()
class AItemCursor : public AItem {
	GENERATED_UCLASS_BODY()
protected:
	terrain::ChunkPositionVector PlayerPosition;
	FMatrix PlayerRotation;
	bool bIsHidden;



	utils::Vector3D<> GetOffsetVector() const;
	virtual void ApplyTransform() override;

public:
	/**
	 * This should be called when the cursor is no longer handling a valid item.
	 */
	void InvalidateCursor();
	void SetPlayerTransform(const utils::Point3D & position, const FMatrix & rotation);
	void SetHidden(const bool isHidden);
	bool IsHidden() const { return bIsHidden; }
	bool IsValid() const { return !!ItemData; }
};

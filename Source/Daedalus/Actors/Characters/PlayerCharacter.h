#pragma once

#include <Actors/Items/ItemCursor.h>
#include <Actors/Terrain/ChunkManager.h>
#include <Controllers/EventBus/EventBus.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Models/Items/ItemData.h>

#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

/**
* Default player character.
*/
UCLASS()
class APlayerCharacter : public ACharacter {
	GENERATED_UCLASS_BODY()

private:
	const float TerrainInteractionDistance;           // Specified in centimetres
	float PositionSecondCount;
	float ViewSecondCount;

	items::ItemDataPtr CurrentHeldItem;
	// Each mouse movement increments this counter, when this is larger than 1, the cursor is
	// updated and this counter is decremented to the fractional component.
	utils::Vector2D<float> MouseHoldOffset;
	items::ItemDataFactoryPtr ItemDataFactory;
	
	events::EventBusPtr EventBusRef;
	const terrain::TerrainGeneratorParameters * TerrainParams;



	void SetUpItemCursor();
	void UpdateItemCursor(const utils::Ray3D & viewpoint);
	void UpdateItemCursorRotation();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent * InputComponent) override;
	virtual void Tick(float delta) override;

	utils::Ray3D GetViewRay() const;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		AChunkManager * ChunkManagerRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		AItemCursor * ItemCursorRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		bool bHoldingJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		bool bRotatingItem;



	UFUNCTION() void MoveForward(float amount);
	UFUNCTION() void MoveRight(float amount);
	UFUNCTION() void LookUp(float amount);
	UFUNCTION() void LookRight(float amount);
	UFUNCTION() void HoldJump();
	UFUNCTION() void ReleaseJump();
	UFUNCTION() void BeginRotation();
	UFUNCTION() void EndRotation();
	UFUNCTION() void PlaceItem();
};


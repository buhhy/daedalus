#pragma once

#include <Actors/Items/ItemCursor.h>
#include <Actors/Terrain/ChunkManager.h>
#include <Controllers/EventBus/EventBus.h>
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
	const float PositionSecondCount;
	const float ViewSecondCount;
	const float TerrainInteractionDistance;           // Specified in centimetres

	const terrain::TerrainGeneratorParameters * TerrainParams;
	utils::Point2D MouseHoldOffset;
	events::EventBusPtr EventBusRef;
	items::ItemDataFactoryPtr ItemDataFactory;



	void SetUpItemCursor();
	void UpdateItemCursor(const utils::Ray3D & viewpoint);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent * InputComponent) override;
	virtual void Tick(float delta) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		AChunkManager * ChunkManagerRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		AItemCursor * ItemCursorRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		bool bHoldingJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		bool bPlacingItem;



	UFUNCTION() void MoveForward(float amount);
	UFUNCTION() void MoveRight(float amount);
	UFUNCTION() void LookUp(float amount);
	UFUNCTION() void LookRight(float amount);
	UFUNCTION() void HoldJump();
	UFUNCTION() void ReleaseJump();
	UFUNCTION() void BeginPlaceItem();
	UFUNCTION() void FinalizePlaceItem();
};


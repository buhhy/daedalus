#pragma once

#include <Actors/Items/ItemCursor.h>
#include <Actors/Terrain/ChunkManager.h>
#include <Controllers/EventBus/EventBus.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Models/Fauna/CharacterData.h>
#include <Models/Fauna/CharacterDataFactory.h>
#include <Models/Items/ItemData.h>
#include <Models/Items/ItemDataFactory.h>

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

	// TODO: assign unique character IDs upon spawning
	fauna::CharacterDataPtr CharDataRef;
	// Each mouse movement increments this counter, when this is larger than 1, the cursor is
	// updated and this counter is decremented to the fractional component.
	utils::Vector2D<float> MouseHoldOffset;

	// TODO: populate these factories from a more global location
	items::ItemDataFactoryPtr ItemDataFactoryRef;
	fauna::CharacterDataFactoryPtr CharDataFactoryRef;
	events::EventBusPtr EventBusRef;
	const terrain::TerrainGeneratorParameters * TerrainParams;



	void SetUpItemCursor();
	void ToggleHandAction();
	void UpdateItemCursorType();
	void UpdateItemCursorPosition();
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

	
	bool canMove() const;

	void MoveForward(float amount);
	void MoveRight(float amount);
	void LookUp(float amount);
	void LookRight(float amount);
	void HoldJump();
	void ReleaseJump();
	void OnRightMouseDown();
	void OnRightMouseUp();
	void OnLeftMouseUp();
	void onEscape();
	void HoldPrevItem();                 // Cycle to the previous inventory item
	void HoldNextItem();                 // Cycle to the next inventory item
};


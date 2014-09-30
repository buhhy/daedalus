#pragma once

#include <Actors/ControllableCharacter.h>
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
class APlayerCharacter : public ACharacter, public IControllableCharacter {
	GENERATED_UCLASS_BODY()
private:
	IControllableCharacter * test;
	const float TerrainInteractionDistance;           // Specified in centimetres
	float PositionSecondCount;
	float ViewSecondCount;

	// TODO: assign unique character IDs upon spawning
	fauna::CharacterDataPtr charDataRef;
	// Each mouse movement increments this counter, when this is larger than 1, the cursor is
	// updated and this counter is decremented to the fractional component.
	utils::Vector2D<float> mouseHoldOffset;

	// TODO: populate these factories from a more global location
	items::ItemDataFactoryPtr itemDataFactoryRef;
	fauna::CharacterDataFactoryPtr charDataFactoryRef;
	events::EventBusPtr eventBusRef;
	const terrain::TerrainGeneratorParameters * terrainParams;



	void SetUpItemCursor();
	void UpdateItemCursorType();
	void UpdateItemCursorPosition();
	void UpdateItemCursorRotation();

protected:
	virtual void BeginPlay() override;
	//virtual void SetupPlayerInputComponent(class UInputComponent * InputComponent) override;
	virtual void Tick(float delta) override;

	utils::Ray3D GetViewRay() const;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		AChunkManager * ChunkManagerRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		AItemCursor * ItemCursorRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
		TSubobjectPtr<UCameraComponent> CameraComponentRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		bool bHoldingJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
		bool bRotatingItem;

	

	fauna::CharacterDataPtr getCharacterData() { return charDataRef; }
	const fauna::CharacterDataPtr getCharacterData() const { return charDataRef; }

	bool canMove() const;

	// External methods for issuing commands to the character entity.
	virtual void moveForward(float amount) override;
	virtual void moveRight(float amount) override;
	virtual void lookUp(float amount) override;
	virtual void lookRight(float amount) override;
	virtual void holdJump() override;
	virtual void releaseJump() override;
	virtual void rightMouseDown() override;
	virtual void rightMouseUp() override;
	virtual void leftMouseDown() override;
	virtual void leftMouseUp() override;
	virtual void escapeKey() override;
	virtual void toggleHandAction() override;
};


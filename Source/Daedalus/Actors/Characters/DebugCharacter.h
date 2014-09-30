#pragma once

#include <Actors/ControllableCharacter.h>

#include "GameFramework/Character.h"
#include "DebugCharacter.generated.h"

/**
* Default player character.
*/
UCLASS()
class ADebugCharacter : public ACharacter, public IControllableCharacter {
	GENERATED_UCLASS_BODY()
private:
	float TickDeltaCount;

	virtual void BeginPlay() override;

	UFUNCTION() void MoveUp(float amount);
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent * InputComponent) override;
	//virtual void Tick(float delta) override;
	
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

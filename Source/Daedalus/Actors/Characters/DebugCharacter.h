#pragma once

#include "GameFramework/Character.h"
#include "DebugCharacter.generated.h"

/**
* Default player character.
*/
UCLASS()
class ADebugCharacter : public ACharacter {
	GENERATED_UCLASS_BODY()
private:
	float TickDeltaCount;

	virtual void BeginPlay() override;

	UFUNCTION() void MoveForward(float amount);
	UFUNCTION() void MoveRight(float amount);
	UFUNCTION() void MoveUp(float amount);
	UFUNCTION() void LookUp(float amount);
	UFUNCTION() void LookRight(float amount);
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent * InputComponent) override;
	//virtual void Tick(float delta) override;
};

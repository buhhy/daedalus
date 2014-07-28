#pragma once

#include <Controllers/EventBus/EventBus.h>

#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

/**
* Default player character.
*/
UCLASS()
class APlayerCharacter : public ACharacter {
	GENERATED_UCLASS_BODY()
private:
	float PositionSecondCount;
	float ViewSecondCount;
	utils::Point2D MouseHoldOffset;
	events::EventBusPtr EventBusRef;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent * InputComponent) override;
	virtual void Tick(float delta) override;

public:
	UPROPERTY(VisibleAnywhere, Category = State)
		bool bHoldingJump;
	UPROPERTY(VisibleAnywhere, Category = State)
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


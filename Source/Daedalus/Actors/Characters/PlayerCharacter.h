#pragma once

#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

/**
* Default player character.
*/
UCLASS()
class APlayerCharacter : public ACharacter {
	GENERATED_UCLASS_BODY()
private:
	float TickDeltaCount;

	virtual void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent * InputComponent) override;
	//virtual void Tick(float delta) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Data)
		bool bHoldingJump;

	UFUNCTION() void MoveForward(float amount);
	UFUNCTION() void MoveRight(float amount);
	UFUNCTION() void LookUp(float amount);
	UFUNCTION() void LookRight(float amount);
	UFUNCTION() void HoldJump();
	UFUNCTION() void ReleaseJump();
};


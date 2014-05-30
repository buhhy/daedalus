#pragma once

#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class APlayerCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	float TickDeltaCount;

	virtual void BeginPlay() override;

	UFUNCTION() void MoveForward(float amount);
	UFUNCTION() void MoveRight(float amount);
	UFUNCTION() void LookUp(float amount);
	UFUNCTION() void LookRight(float amount);
	UFUNCTION() void HoldJump();
	UFUNCTION() void ReleaseJump();
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent * InputComponent) override;
	virtual void Tick(float delta) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Data)
		bool bHoldingJump;
};

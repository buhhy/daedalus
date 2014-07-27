#pragma once

#include "GameFramework/PlayerController.h"
#include "DDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ADDPlayerController : public APlayerController {
	GENERATED_UCLASS_BODY()
private:
	float PositionSecondCount;
	float ViewSecondCount;

protected:
	virtual void Tick(float delta) override;

public:
};

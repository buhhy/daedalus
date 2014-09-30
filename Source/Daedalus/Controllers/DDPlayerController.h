#pragma once

#include <GameFramework/PlayerController.h>
#include <Actors/ControllableCharacter.h>
#include <Actors/SlateUI/PlayerHUD.h>
#include "DDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ADDPlayerController : public APlayerController {
	GENERATED_UCLASS_BODY()
private:
	APlayerHUD * playerHUD;



	IControllableCharacter * getOwnedCharacter();
	utils::Point2D ADDPlayerController::getMousePosition() const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	void setHUDDashboardOpen(const bool isOpen);
	bool isHUDDashboardOpen() const;
	
	void setMainMenuOpen(const bool isOpen);
	bool isMainMenuOpen() const;

	
	// Input binding functions.
	void moveForward(float amount);
	void moveRight(float amount);
	void lookUp(float amount);
	void lookRight(float amount);
	void onHoldJump();
	void onReleaseJump();
	void onRightMouseDown();
	void onRightMouseUp();
	void onLeftMouseDown();
	void onLeftMouseUp();
	void onEscape();
	void onToggleHandAction();
	void onToggleDashboardUI();
};

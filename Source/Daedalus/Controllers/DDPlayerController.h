#pragma once

#include <GameFramework/PlayerController.h>
#include <Actors/ControllableCharacter.h>
#include <Actors/GUI/PlayerHUD.h>
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
	virtual bool InputKey(FKey key, EInputEvent evtType, float amtDep, bool bGamepad) override;

public:
	void setHUDDashboardOpen(const bool isOpen);
	bool isHUDDashboardOpen() const;
	
	void setMainMenuOpen(const bool isOpen);
	bool isMainMenuOpen() const;

	
	// Input binding functions.
	void moveForward(const float amount);
	void moveRight(const float amount);
	void lookUp(const float amount);
	void lookRight(const float amount);
	void onQuickUse(const Uint8);
	void onHoldJump();
	void onReleaseJump();
	void onRightMouseDown();
	void onRightMouseUp();
	void onLeftMouseDown();
	void onLeftMouseUp();
	void onEscape();
	void onToggleHandAction();
	void onToggleDashboardUI();
	
	// Because we must bind a function with no arguments...
	template <Uint8 T>
	void onQuickUse() { onQuickUse(T); }
};

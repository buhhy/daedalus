#include <Daedalus.h>
#include <Controllers/DDPlayerController.h>

#include <Actors/Characters/PlayerCharacter.h>
#include <Controllers/DDGameState.h>
#include <Utilities/UnrealBridge.h>

#include <functional>

using namespace events;
using namespace utils;
using namespace gui;

ADDPlayerController::ADDPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{}

Point2D ADDPlayerController::getMousePosition() const {
	float x, y;
	GetMousePosition(x, y);
	return Point2D(x, y);
}

void ADDPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();
	auto & ins = InputComponent;

	ins->BindAxis("MoveForward", this, &ADDPlayerController::moveForward);
	ins->BindAxis("MoveRight", this, &ADDPlayerController::moveRight);

	ins->BindAxis("LookUp", this, &ADDPlayerController::lookUp);
	ins->BindAxis("LookRight", this, &ADDPlayerController::lookRight);

	ins->BindAction("Jump", IE_Pressed, this, &ADDPlayerController::onHoldJump);
	ins->BindAction("Jump", IE_Released, this, &ADDPlayerController::onReleaseJump);

	ins->BindAction("RightMouseClick", IE_Pressed, this, &ADDPlayerController::onRightMouseDown);
	ins->BindAction("RightMouseClick", IE_Released, this, &ADDPlayerController::onRightMouseUp);
	ins->BindAction("LeftMouseClick", IE_Pressed, this, &ADDPlayerController::onLeftMouseDown);
	ins->BindAction("LeftMouseClick", IE_Released, this, &ADDPlayerController::onLeftMouseUp);
	ins->BindAction("Escape", IE_Released, this, &ADDPlayerController::onEscape);
	ins->BindAction("ToggleHandAction", IE_Released, this, &ADDPlayerController::onToggleHandAction);
	ins->BindAction("ToggleDashboard", IE_Released, this, &ADDPlayerController::onToggleDashboardUI);
	
	ins->BindAction("Quickuse1", IE_Pressed, this, &ADDPlayerController::onQuickUse<0>);
	ins->BindAction("Quickuse2", IE_Pressed, this, &ADDPlayerController::onQuickUse<1>);
	ins->BindAction("Quickuse3", IE_Pressed, this, &ADDPlayerController::onQuickUse<2>);
	ins->BindAction("Quickuse4", IE_Pressed, this, &ADDPlayerController::onQuickUse<3>);
	ins->BindAction("Quickuse5", IE_Pressed, this, &ADDPlayerController::onQuickUse<4>);
	ins->BindAction("Quickuse6", IE_Pressed, this, &ADDPlayerController::onQuickUse<5>);
	ins->BindAction("Quickuse7", IE_Pressed, this, &ADDPlayerController::onQuickUse<6>);
	ins->BindAction("Quickuse8", IE_Pressed, this, &ADDPlayerController::onQuickUse<7>);
}

void ADDPlayerController::BeginPlay() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, TEXT("Using DDPlayerController"));

	playerHUD = Cast<APlayerHUD>(GetHUD());
}

bool ADDPlayerController::InputKey(
	FKey key, EInputEvent evtType,
	float amtDep, bool bGamepad
) {
	bool ret = APlayerController::InputKey(key, evtType, amtDep, bGamepad);
	
	// Check for keyboard input for text areas.
	/*if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
	} else {
	}*/

	return ret;
}

void ADDPlayerController::setHUDDashboardOpen(const bool isOpen) {
	playerHUD->setDashboardOpen(isOpen);
	SetIgnoreLookInput(isOpen);
	if (!isOpen)
		FSlateApplication::Get().SetFocusToGameViewport();
}

bool ADDPlayerController::isHUDDashboardOpen() const {
	return playerHUD->isDashboardOpen();
}

void ADDPlayerController::setMainMenuOpen(const bool isOpen) {
	SetIgnoreLookInput(isOpen);
}

bool ADDPlayerController::isMainMenuOpen() const {
	return false;
}



/********************************************************************************
 * Method bindings for the input component
 * ---------------------------------------
 * The inputs are provided for these components in the following order:
 * main menu, dashboard, player character. If any of the menus are opened, they
 * will consume the input, breaking the cascade.
 ********************************************************************************/

IControllableCharacter * ADDPlayerController::getOwnedCharacter() {
	auto t =  GetPawn();
	return InterfaceCast<IControllableCharacter>(t);
}

void ADDPlayerController::moveForward(float amount) {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->moveForward(amount);
	}
}

void ADDPlayerController::moveRight(float amount) {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->moveRight(amount);
	}
}

void ADDPlayerController::lookUp(float amount) {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
		playerHUD->onMouseMove(getMousePosition());
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->lookUp(amount);
	}
}

void ADDPlayerController::lookRight(float amount) {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
		playerHUD->onMouseMove(getMousePosition());
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->lookRight(amount);
	}
}

void ADDPlayerController::onQuickUse(const Uint8 slot) {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->quickuse(slot);
	}
}

void ADDPlayerController::onHoldJump() {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->holdJump();
	}
}

void ADDPlayerController::onReleaseJump() {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->releaseJump();
	}
}

void ADDPlayerController::onRightMouseDown() {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
		playerHUD->onMouseDown(MouseEvent::BUTTON_PRESS_RIGHT);
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->rightMouseDown();
	}
}

void ADDPlayerController::onRightMouseUp() {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
		playerHUD->onMouseUp(MouseEvent::BUTTON_PRESS_RIGHT);
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->rightMouseUp();
	}
}

void ADDPlayerController::onLeftMouseDown() {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
		playerHUD->onMouseDown(MouseEvent::BUTTON_PRESS_LEFT);
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->leftMouseDown();
	}
}

void ADDPlayerController::onLeftMouseUp() {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
		playerHUD->onMouseUp(MouseEvent::BUTTON_PRESS_LEFT);
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->leftMouseUp();
	}
}

void ADDPlayerController::onEscape() {
	if (isMainMenuOpen()) {
		setMainMenuOpen(false);
	} else if (isHUDDashboardOpen()) {
		setHUDDashboardOpen(false);
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->escapeKey();
	}
}

void ADDPlayerController::onToggleDashboardUI() {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
		setHUDDashboardOpen(false);
	} else {
		setHUDDashboardOpen(true);
	}
}

void ADDPlayerController::onToggleHandAction() {
	if (isMainMenuOpen()) {
	} else if (isHUDDashboardOpen()) {
	} else {
		auto ch = getOwnedCharacter();
		if (ch)
			ch->toggleHandAction();
	}
}

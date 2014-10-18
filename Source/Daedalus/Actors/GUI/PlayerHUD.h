#pragma once

#include "GameFramework/HUD.h"

#include <Actors/GUI/BasicElements.h>
#include <Actors/GUI/CursorElement.h>
#include <Actors/GUI/DashboardElements.h>
#include <Models/Fauna/CharacterData.h>

#include "PlayerHUD.generated.h"

class ADDGameState;

UCLASS()
class APlayerHUD : public AHUD {
	GENERATED_UCLASS_BODY()
private:
	bool bDashboardOpen;
	Uint8 mouseButtonsActive;
	utils::Point2D cursorPosition;
	gui::MouseEvent::CursorType currentCursorType;
	gui::MouseEvent::CursorType previousCursorType;

	gui::CursorElementPtr cursorNode;
	gui::DocumentRootElementPtr rootNode;
	gui::DivElementPtr dashboardRootNode;
	gui::InventoryElementPtr inventoryNode;
	gui::QuickuseBarElementPtr quickbarNode;



	void drawDefaultHUDElements(
		fauna::CharacterDataPtr & characterData, ADDGameState * gameState);
	void drawDashboardElements(
		fauna::CharacterDataPtr & characterData, ADDGameState * gameState);

protected:
	//TSharedPtr<SInventoryPanel> InventoryUI;
	
	virtual void DrawHUD() override;

public:
	virtual void PostInitializeComponents() override;

	void setDashboardOpen(const bool isOpen);
	bool isDashboardOpen() const;

	void onMouseMove(const utils::Point2D & position);
	void onMouseDown(const Uint8 whichBtn);
	void onMouseUp(const Uint8 whichBtn);
};
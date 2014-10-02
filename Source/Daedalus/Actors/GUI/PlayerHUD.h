//Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
//Author: Bleakwise
//File: MyHUD.h

#pragma once

#include "GameFramework/HUD.h"

#include <Actors/GUI/InventoryPanel.h>
#include <Actors/GUI/HUDElement.h>
#include <Models/Fauna/CharacterData.h>

#include "PlayerHUD.generated.h"

class ADDGameState;

UCLASS()
class APlayerHUD : public AHUD {
	GENERATED_UCLASS_BODY()
public:
	enum CursorType {
		C_Pointer,
		C_Hover,
		C_Active
	};

	static const Uint8 BUTTON_PRESS_LEFT = (Uint8) 0x1;
	static const Uint8 BUTTON_PRESS_MIDDLE = (Uint8) 0x2;
	static const Uint8 BUTTON_PRESS_RIGHT = (Uint8) 0x4;

private:
	bool bDashboardOpen;
	Uint8 mouseButtonsActive;
	utils::Point2D cursorPosition;
	CursorType currentCursorType;
	CursorType previousCursorType;

	gui::HUDElementPtr rootNode;
	gui::QuickbarPtr quickbarNode;



	void drawDefaultHUDElements(
		fauna::CharacterDataPtr & characterData, ADDGameState * gameState);
	void drawDashboardElements(
		fauna::CharacterDataPtr & characterData, ADDGameState * gameState);
	void drawCursor(ADDGameState * gameState);

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
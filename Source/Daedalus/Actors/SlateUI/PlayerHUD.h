//Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
//Author: Bleakwise
//File: MyHUD.h

#pragma once

#include "GameFramework/HUD.h"

#include <Actors/SlateUI/InventoryPanel.h>
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

private:
	UFont * uiFontLatoSmall;

	bool bDashboardOpen;
	bool bMouseDown;
	utils::Point2D cursorPosition;
	CursorType currentCursorType;
	CursorType previousCursorType;



	void drawDefaultHUDElements(
		const fauna::CharacterDataPtr & characterData, ADDGameState * gameState);
	void drawDashboardElements(
		const fauna::CharacterDataPtr & characterData, ADDGameState * gameState);
	void drawCursor(ADDGameState * gameState);

protected:
	//TSharedPtr<SInventoryPanel> InventoryUI;
	
	virtual void DrawHUD() override;

public:
	virtual void PostInitializeComponents() override;

	void setDashboardOpen(const bool isOpen);
	bool isDashboardOpen() const;

	void onMouseMove(const utils::Point2D & position);
	void onMouseDown();
	void onMouseUp();
};
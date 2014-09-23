//Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
//Author: Bleakwise
//File: MyHUD.h

#pragma once

#include "GameFramework/HUD.h"

#include <Actors/SlateUI/InventoryPanel.h>
#include <Models/Fauna/CharacterData.h>

#include "PlayerHUD.generated.h"

UCLASS()
class APlayerHUD : public AHUD {
	GENERATED_UCLASS_BODY()
private:
	UFont * uiFontLatoSmall;

protected:
	//TSharedPtr<SInventoryPanel> InventoryUI;
	
	virtual void DrawHUD() override;

public:
	virtual void PostInitializeComponents() override;

	UFUNCTION() void OpenPlayerInventory();
	UFUNCTION() void ClosePlayerInventory();
};
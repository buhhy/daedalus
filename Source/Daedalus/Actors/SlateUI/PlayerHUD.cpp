#include <Daedalus.h>
#include "PlayerHUD.h"


APlayerHUD::APlayerHUD(const class FPostConstructInitializeProperties & PCIP) : Super(PCIP) {

}

void APlayerHUD::OpenPlayerInventory() {
}

void APlayerHUD::ClosePlayerInventory() {
}

void APlayerHUD::PostInitializeComponents() {
	Super::PostInitializeComponents();

	SAssignNew(InventoryUI, SInventoryPanel).OwningHUD(this);

	// Pass our viewport a weak ptr to our widget
	if (GEngine->IsValidLowLevel()) {
		// Viewport's weak ptr will not give Viewport ownership of Widget
		GEngine->GameViewport->AddViewportWidgetContent(
			SNew(SWeakWidget).PossiblyNullContent(InventoryUI.ToSharedRef()));
	}

	if (InventoryUI.IsValid()) {
		// Set widget's properties as visible (sets child widget's properties recursively)
		InventoryUI->SetVisibility(EVisibility::Visible);
	}
}
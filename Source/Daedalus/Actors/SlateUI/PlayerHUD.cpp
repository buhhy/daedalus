#include <Daedalus.h>
#include "PlayerHUD.h"
#include <Actors/Characters/PlayerCharacter.h>


APlayerHUD::APlayerHUD(const class FPostConstructInitializeProperties & PCIP) : Super(PCIP) {
	static ConstructorHelpers::FObjectFinder<UFont> latoSmall(
		TEXT("Font'/Game/Lato-Small.Lato-Small'"));
	uiFontLatoSmall = latoSmall.Object;
}

void APlayerHUD::OpenPlayerInventory() {
}

void APlayerHUD::ClosePlayerInventory() {
}

void APlayerHUD::PostInitializeComponents() {
	Super::PostInitializeComponents();

	//SAssignNew(InventoryUI, SInventoryPanel).OwningHUD(this);

	// Pass our viewport a weak ptr to our widget
	if (GEngine->IsValidLowLevel()) {
		// Viewport's weak ptr will not give Viewport ownership of Widget
		//GEngine->GameViewport->AddViewportWidgetContent(
		//	SNew(SWeakWidget).PossiblyNullContent(InventoryUI.ToSharedRef()));
	}

	//if (InventoryUI.IsValid()) {
	//	// Set widget's properties as visible (sets child widget's properties recursively)
	//	InventoryUI->SetVisibility(EVisibility::Visible);
	//}
}

void APlayerHUD::DrawHUD() {
	Super::DrawHUD();

	fauna::CharacterDataPtr characterData;
	auto controller = GetOwningPlayerController();
	auto gameState = GetWorld()->GetGameState<ADDGameState>();

	if (controller && gameState) {
		auto character = (APlayerCharacter *) controller->GetCharacter();
		if (character) {
			characterData = character->getCharacterData();
		}
	}

	if (characterData) {
		const auto resourceCache = gameState->getResourceCacheRef();
		const auto screenW = Canvas->SizeX;
		const auto screenH = Canvas->SizeY;

		std::stringstream ss;
		ss << characterData->getCurrentHP() << "/" << characterData->getDefaultMaxHP();
		const auto healthStr = FString(UTF8_TO_TCHAR(ss.str().c_str()));
		float healthStrWidth, healthStrHeight;
		GetTextSize(healthStr, healthStrWidth, healthStrHeight, uiFontLatoSmall);
		
		ss.str(""); ss.clear();

		// Draw stuff only if the player controller is actually possessing a character.
		DrawRect(FLinearColor(0, 0.85, 0.2), 0, 0, screenW, 20);
		DrawText(healthStr, FLinearColor(1, 1, 1),
			(screenW - healthStrWidth) / 2, 2, uiFontLatoSmall);

		// Draw shortcut bar.
		const float slotSize = 64, slotBorder = 4;
		const auto curSBar = characterData->getCurrentShortcutSet();
		const auto barCount = curSBar->getMaxSize();
		Uint32 i = 0;
		for (auto it = curSBar->startIterator(); it != curSBar->endIterator(); ++it) {
			float x = screenW / 2 + (i - barCount / 2.0) * (slotSize + slotBorder) + slotBorder / 2;
			float y = screenH - (slotSize + slotBorder);
			if (*it) {
				auto icon = resourceCache->findIcon((*it)->getIconName());
				DrawTextureSimple(icon, x, y, slotSize / icon->GetSizeX());
			} else {
				DrawRect(FLinearColor(0.9, 0.9, 0.9), x, y, slotSize, slotSize);
			}
			i++;
		}
	}
}

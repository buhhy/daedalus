#include <Daedalus.h>
#include <Actors/GUI/PlayerHUD.h>
#include <Actors/Characters/PlayerCharacter.h>
#include <Controllers/DDGameState.h>

using namespace fauna;
using namespace utils;

APlayerHUD::APlayerHUD(const class FPostConstructInitializeProperties & PCIP) :
	Super(PCIP), cursorPosition(0, 0), bDashboardOpen(false), mouseButtonsActive(0),
	currentCursorType(C_Pointer), previousCursorType(C_Pointer)
{
	static ConstructorHelpers::FObjectFinder<UFont> latoSmall(
		TEXT("Font'/Game/Lato-Small.Lato-Small'"));
	uiFontLatoSmall = latoSmall.Object;
}

void APlayerHUD::drawDefaultHUDElements(
	const CharacterDataPtr & characterData,
	ADDGameState * gameState
) {
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
		if (*it && (*it)->isValid()) {
			auto icon = resourceCache->findIcon((*it)->getIconName());
			auto count = (*it)->getQuantity();
			DrawTexture(icon, x, y, slotSize, slotSize, 0, 0, 1.0, 1.0);
			if (count.IsValid()) {
				ss << *count;
				DrawText(FString(UTF8_TO_TCHAR(ss.str().c_str())),
					FLinearColor(1, 1, 1), x + 5, y + 5, uiFontLatoSmall);
				ss.str(""); ss.clear();
			}
		} else {
			DrawRect(FLinearColor(0.5, 0.5, 0.5), x, y, slotSize, slotSize);
		}
		i++;
	}
}

void APlayerHUD::drawDashboardElements(
	const CharacterDataPtr & characterData,
	ADDGameState * gameState
) {
	const auto resourceCache = gameState->getResourceCacheRef();
	const auto screenW = Canvas->SizeX;
	const auto screenH = Canvas->SizeY;

	DrawText(FString(TEXT("OPENED")), FLinearColor(1, 1, 1), screenW / 2, screenH / 2);
}

void APlayerHUD::drawCursor(ADDGameState * gameState) {
	const auto resourceCache = gameState->getResourceCacheRef();

	std::string cursorSuffix;
	if (mouseButtonsActive & BUTTON_PRESS_LEFT)
		cursorSuffix = "-Active";
	//switch (currentCursorType) {
	//case C_Hover:
	//	cursorSuffix = "-Hover";
	//	break;
	//case C_Active:
	//	cursorSuffix = "-Active";
	//	break;
	//default:
	//	break;
	//}

	UTexture2D * cursorTexture = resourceCache->findIcon(
		"Pointer" + cursorSuffix, ResourceCache::ICON_CURSOR_FOLDER);

	DrawTextureSimple(cursorTexture, cursorPosition.X, cursorPosition.Y);
}

void APlayerHUD::DrawHUD() {
	Super::DrawHUD();

	CharacterDataPtr characterData;
	auto controller = GetOwningPlayerController();
	auto gameState = GetWorld()->GetGameState<ADDGameState>();

	if (controller && gameState) {
		auto character = (APlayerCharacter *) controller->GetCharacter();
		if (character) {
			characterData = character->getCharacterData();
		}
	}

	if (characterData) {
		drawDefaultHUDElements(characterData, gameState);
		if (bDashboardOpen) {
			drawDashboardElements(characterData, gameState);
			drawCursor(gameState);
		}
	}
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

void APlayerHUD::setDashboardOpen(const bool isOpen) {
	bDashboardOpen = isOpen;
}

bool APlayerHUD::isDashboardOpen() const {
	return bDashboardOpen;
}

void APlayerHUD::onMouseMove(const Point2D & position) {
	cursorPosition = position;
}

void APlayerHUD::onMouseDown(const Uint8 whichBtn) {
	mouseButtonsActive |= whichBtn;
}

void APlayerHUD::onMouseUp(const Uint8 whichBtn) {
	mouseButtonsActive ^= whichBtn;
}

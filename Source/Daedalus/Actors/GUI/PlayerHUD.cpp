#include <Daedalus.h>
#include <Actors/GUI/PlayerHUD.h>
#include <Actors/Characters/PlayerCharacter.h>
#include <Controllers/DDGameState.h>

using namespace fauna;
using namespace utils;
using namespace gui;

APlayerHUD::APlayerHUD(const class FPostConstructInitializeProperties & PCIP) :
	Super(PCIP), cursorPosition(0, 0), bDashboardOpen(false), mouseButtonsActive(0),
	currentCursorType(MouseEvent::C_Pointer), previousCursorType(MouseEvent::C_Pointer),
	cursorNode(new CursorElement()),
	rootNode(new DocumentRootElement(cursorNode)),
	dashboardRootNode(new DivElement()),
	quickbarNode(new QuickuseBarElement(cursorNode)),
	inventoryNode(new InventoryElement(cursorNode))
{}

void APlayerHUD::drawDefaultHUDElements(
	CharacterDataPtr & characterData,
	ADDGameState * gameState
) {
	const auto resourceCache = gameState->getResourceCacheRef();
	const auto screenW = Canvas->SizeX;
	const auto screenH = Canvas->SizeY;
	auto uiFontLatoSmall = resourceCache->findFont("Lato", 12);

	rootNode->resize({ (double) screenW, (double) screenH });
	dashboardRootNode->resize({ (double) screenW, (double) screenH });

	quickbarNode->updateQuickbar(characterData->getCurrentShortcutSet());
	inventoryNode->updateData(
		characterData->getInventory(), characterData->getCurrentShortcutSet()->getMaxSize());

	rootNode->runLogic(0);
	rootNode->drawElementTree(this, resourceCache);

	// TODO: make this into an element.
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
}

void APlayerHUD::drawDashboardElements(
	CharacterDataPtr & characterData,
	ADDGameState * gameState
) {
	const auto resourceCache = gameState->getResourceCacheRef();
	const auto screenW = Canvas->SizeX;
	const auto screenH = Canvas->SizeY;

	DrawText(FString(TEXT("OPENED")), FLinearColor(1, 1, 1), screenW / 2, screenH / 2);
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


	// Set up GUI.
	rootNode->appendChild(quickbarNode);
	rootNode->appendChild(dashboardRootNode);
	dashboardRootNode->appendChild(inventoryNode);
	dashboardRootNode->appendChild(cursorNode);
	setDashboardOpen(false);
}

void APlayerHUD::setDashboardOpen(const bool isOpen) {
	bDashboardOpen = isOpen;
	dashboardRootNode->setHidden(!isOpen);
}

bool APlayerHUD::isDashboardOpen() const {
	return bDashboardOpen;
}

void APlayerHUD::onMouseMove(const Point2D & position) {
	cursorPosition = position;
	rootNode->checkMouseMove(position);
}

void APlayerHUD::onMouseDown(const Uint8 whichBtn) {
	mouseButtonsActive |= whichBtn;
	rootNode->checkMouseDown(MouseEvent(cursorPosition, whichBtn));
}

void APlayerHUD::onMouseUp(const Uint8 whichBtn) {
	mouseButtonsActive ^= whichBtn;
	rootNode->checkMouseUp(MouseEvent(cursorPosition, whichBtn));
}

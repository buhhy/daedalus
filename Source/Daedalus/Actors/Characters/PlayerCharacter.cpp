#include "Daedalus.h"
#include "PlayerCharacter.h"

#include <Actors/SlateUI/PlayerHUD.h>
#include <Controllers/DDPlayerController.h>
#include <Controllers/DDGameState.h>
#include <Utilities/Constants.h>
#include <Utilities/UnrealBridge.h>

using namespace utils;
using namespace events;
using namespace items;
using namespace terrain;
using namespace fauna;

APlayerCharacter::APlayerCharacter(const class FPostConstructInitializeProperties& PCIP) :
	Super(PCIP), bHoldingJump(false), mouseHoldOffset(0, 0),
	PositionSecondCount(0), ViewSecondCount(0), TerrainInteractionDistance(250),
	itemDataFactoryRef(new ItemDataFactory()),
	charDataFactoryRef(new CharacterDataFactory())
{
	auto & movement = this->CharacterMovement;
	movement->SetWalkableFloorAngle(60.0);
	movement->JumpZVelocity = 400;
	movement->GravityScale = 2.8;
	movement->AirControl = 0.4;

	// Set up the default camera.
	CameraComponentRef = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	CameraComponentRef->AttachParent = CapsuleComponent;
	CameraComponentRef->AddLocalOffset(FVector(-100, 0, 200));

	charDataRef = charDataFactoryRef->BuildCharData(C_Hero);
	// TODO: remove this test code
	charDataRef->inventoryRef->AddItems(itemDataFactoryRef->BuildItemData(I_Sofa), 3);
	charDataRef->inventoryRef->AddItems(itemDataFactoryRef->BuildItemData(I_Chest), 10);
	charDataRef->addToCurrentShortcutSet(charDataRef->getItemInInventory(0), 0);
	charDataRef->addToCurrentShortcutSet(charDataRef->getItemInInventory(1), 1);
}

utils::Ray3D APlayerCharacter::GetViewRay() const {
	FVector pos;
	FRotator dir;
		
	GetActorEyesViewPoint(pos, dir);
	return Ray3D(ToVector3D(pos), ToVector3D(dir.Vector()));
}

void APlayerCharacter::SetUpItemCursor() {
	const FRotator defaultRotator(0, 0, 0);
	auto params = FActorSpawnParameters();
	params.Name = TEXT("ItemPlacementGhostCursor");
	ItemCursorRef = GetWorld()->SpawnActor<AItemCursor>(
		AItemCursor::StaticClass(), { 0, 0, 0 }, { 0, 0, 0 }, params);
	ItemCursorRef->AttachRootComponentToActor(this);
}

void APlayerCharacter::UpdateItemCursorType() {
	switch (charDataRef->getCurrentHandAction()) {
	case H_None:
		ItemCursorRef->InvalidateCursor();
		break;
	case H_Item: {
		const auto item = charDataRef->getCurrentItemInInventory();
		if (item)
			ItemCursorRef->initialize(item);
		else
			ItemCursorRef->InvalidateCursor();
		break;
	}
	case H_Tool:
	default:
		break;
	}
}

void APlayerCharacter::UpdateItemCursorPosition() {
	bool hidden = true;
	
	if (ItemCursorRef->IsValid()) {
		const auto viewpoint = GetViewRay();
		const auto foundResult =
			ChunkManagerRef->Raytrace(viewpoint, TerrainInteractionDistance);

		if (foundResult.IsValid()) {
			const auto & deref = *foundResult;
			if (deref.Type == E_Terrain || deref.Type == E_PlacedItem) {
				ItemCursorRef->getItemData()->setPosition(deref.EntryPosition);
				hidden = false;
			}
		}
	}
	ItemCursorRef->SetHidden(hidden);
}

void APlayerCharacter::UpdateItemCursorRotation() {
	if (ItemCursorRef->IsValid()) {
		// Find out how many notches to turn
		Vector2D<Int16> turnNotches;
		for (Uint8 i = 0; i < 2; i++) {
			turnNotches[i] = (Int16) mouseHoldOffset[i];
			if (turnNotches[i] != 0)
				mouseHoldOffset[i] -= turnNotches[i];
		}

		if (!EEq(turnNotches.Length2(), 0)) {
			const auto & item = ItemCursorRef->getItemData();
			const auto yaw = item->Template.rotationInterval.Yaw;
			const auto pitch = item->Template.rotationInterval.Pitch;

			// Bind the notches, since the turn interval only accepts unsigned integers
			while (turnNotches.X < 0)
				turnNotches.X += yaw;
			while (turnNotches.X < 0)
				turnNotches.Y += pitch;

			turnNotches.X %= yaw;
			turnNotches.Y %= pitch;

			ItemRotation rot(-turnNotches.X, turnNotches.Y);
			ItemCursorRef->getItemData()->AddRotation(rot);
		}
	}
}

bool APlayerCharacter::canMove() const {
	auto usingItem = charDataRef->getUsingItem();
	if (!usingItem.expired()) {
		const auto item = ChunkManagerRef->FindPlacedItem(usingItem.lock()->getItemId());
		// Check if the item in use still exists.
		return !item;
	}
	return true;
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, TEXT("Using PlayerCharacter"));

	auto gameState = GetWorld()->GetGameState<ADDGameState>();

	assert(gameState && "APlayerCharacter::BeginPlay: Game state is NULL here.");

	eventBusRef = gameState->EventBus;
	ChunkManagerRef = *(TActorIterator<AChunkManager>(GetWorld())); // There should always be a chunk manager
	terrainParams = &gameState->ChunkLoader->GetGeneratorParameters();

	SetUpItemCursor();
}

//void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent * inputComp) {
//	Super::SetupPlayerInputComponent(inputComp);
//}

void APlayerCharacter::Tick(float delta) {
	Super::Tick(delta);
	PositionSecondCount += delta;
	ViewSecondCount += delta;

	// Tick once every 20 milliseconds
	if (ViewSecondCount >= 0.02) {
		ViewSecondCount -= 0.02;

		ItemCursorRef->SetPlayerTransform(
			ToVector3D(this->GetActorLocation()), FRotationMatrix(this->GetActorRotation()));

		UpdateItemCursorType();
		UpdateItemCursorRotation();
		UpdateItemCursorPosition();

		eventBusRef->BroadcastEvent(EventDataPtr(new EViewPosition(GetViewRay())));
	}

	// Tick once every half-second
	if (PositionSecondCount >= 0.5) {
		PositionSecondCount -= 0.5;
		
		eventBusRef->BroadcastEvent(
			EventDataPtr(new EPlayerPosition(ToVector3D(GetActorLocation()))));
	}
}



/********************************************************************************
 * Implementation for DDPlayerController
 ********************************************************************************/

void APlayerCharacter::moveForward(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > FLOAT_ERROR && canMove()) {
		FRotator rotator = Controller->GetControlRotation();

		if (CharacterMovement->IsMovingOnGround() || CharacterMovement->IsFalling())
			rotator.Pitch = 0.0;

		const FVector direction = FRotationMatrix(rotator).GetScaledAxis(EAxis::X);
		AddMovementInput(direction, amount);
	}
}

void APlayerCharacter::moveRight(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > FLOAT_ERROR && canMove()) {
		FRotator rotator = Controller->GetControlRotation();
		const FVector direction = FRotationMatrix(rotator).GetScaledAxis(EAxis::Y);
		AddMovementInput(direction, amount);
	}
}

void APlayerCharacter::lookUp(float amount) {
	if (bRotatingItem) {
		mouseHoldOffset.Y += amount / 10.0;
	} else {
		AddControllerPitchInput(amount);
	}
}

void APlayerCharacter::lookRight(float amount) {
	if (bRotatingItem) {
		mouseHoldOffset.X += amount / 10.0;
	} else {
		AddControllerYawInput(amount);
	}
}

void APlayerCharacter::holdJump() {
	if (canMove()) {
		bHoldingJump = true;
		bPressedJump = true;
	}
}

void APlayerCharacter::releaseJump() {
	bHoldingJump = false;
	bPressedJump = false;
}

void APlayerCharacter::rightMouseDown() {
	switch (charDataRef->getCurrentHandAction()) {
	case H_None:
		break;
	case H_Item:
		bRotatingItem = true;
		mouseHoldOffset.Reset(0, 0);
		break;
	case H_Tool:
		break;
	}
}

void APlayerCharacter::rightMouseUp() {
	if (bRotatingItem) {
		bRotatingItem = false;
		mouseHoldOffset.Reset(0, 0);
	} else {
		switch (charDataRef->getCurrentHandAction()) {
		case H_None: {
			// TODO: item interactions
			const auto viewpoint = GetViewRay();
			const auto foundResult =
				ChunkManagerRef->Raytrace(viewpoint, TerrainInteractionDistance);

			if (foundResult.IsValid()) {
				const auto & deref = *foundResult;
				if (deref.Type == E_PlacedItem) {
					deref.ItemData->interactAction(charDataRef);
				}
			}

			break;
		}
		case H_Item:
			break;
		case H_Tool:
			break;
		}
	}
}

void APlayerCharacter::leftMouseDown() {}

void APlayerCharacter::leftMouseUp() {
	switch (charDataRef->getCurrentHandAction()) {
	case H_None:
		break;
	case H_Item:
		if (ItemCursorRef->IsValid() && !ItemCursorRef->IsHidden()) {
			const auto curItem = charDataRef->getCurrentItemInInventory();
			// TODO: if we wish to preserve item state, we'll need to place the original item data here
			if (curItem) {
				if (ChunkManagerRef->PlaceItem(ItemDataPtr(new ItemData(*curItem))))
					charDataRef->placeCurrentItemInInventory();
			}
		}
		break;
	case H_Tool:
		break;
	}
}

void APlayerCharacter::escapeKey() {
	if (charDataRef->isUsingItem()) {
		charDataRef->stopUsingItem();
	}

	// Close the user HUD dashboard if it is open.
	auto playerController = Cast<ADDPlayerController>(Controller);
	if (playerController && playerController->isHUDDashboardOpen())
		playerController->setHUDDashboardOpen(false);
}

void APlayerCharacter::toggleHandAction() {
	switch (charDataRef->getCurrentHandAction()) {
	case H_None:
		charDataRef->switchHandAction(H_Item);
		break;
	case H_Item:
		charDataRef->switchHandAction(H_None);
		break;
	case H_Tool:
	default:
		break;
	}
}

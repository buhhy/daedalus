#include "Daedalus.h"
#include "PlayerCharacter.h"

#include <Controllers/DDGameState.h>
#include <Utilities/Constants.h>
#include <Utilities/UnrealBridge.h>

using namespace utils;
using namespace events;
using namespace items;
using namespace terrain;
using namespace fauna;

APlayerCharacter::APlayerCharacter(const class FPostConstructInitializeProperties& PCIP) :
	Super(PCIP), bHoldingJump(false), MouseHoldOffset(0, 0),
	PositionSecondCount(0), ViewSecondCount(0), TerrainInteractionDistance(250),
	ItemDataFactoryRef(new ItemDataFactory()),
	CharDataFactoryRef(new CharacterDataFactory())
{
	auto & movement = this->CharacterMovement;
	movement->SetWalkableFloorAngle(60.0);
	movement->JumpZVelocity = 400;
	movement->AirControl = 0.4;

	CharDataRef = CharDataFactoryRef->BuildCharData(C_Hero);
	CharDataRef->CharId = 1;
	// TODO: remove this test code
	CharDataRef->InventoryRef->AddItems(ItemDataFactoryRef->BuildItemData(I_Sofa), 3);
	CharDataRef->InventoryRef->AddItems(ItemDataFactoryRef->BuildItemData(I_Chest), 10);
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

void APlayerCharacter::ToggleHandAction() {
	switch (CharDataRef->GetCurrentHandAction()) {
	case H_None:
		CharDataRef->SwitchHandAction(H_Item);
		break;
	case H_Item:
		CharDataRef->SwitchHandAction(H_None);
		break;
	case H_Tool:
	default:
		break;
	}
}

void APlayerCharacter::UpdateItemCursorType() {
	switch (CharDataRef->GetCurrentHandAction()) {
	case H_None:
		ItemCursorRef->InvalidateCursor();
		break;
	case H_Item: {
		const auto item = CharDataRef->GetCurrentItemInInventory();
		if (item)
			ItemCursorRef->Initialize(item);
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
				ItemCursorRef->GetItemData()->setPosition(deref.EntryPosition);
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
			turnNotches[i] = (Int16) MouseHoldOffset[i];
			if (turnNotches[i] != 0)
				MouseHoldOffset[i] -= turnNotches[i];
		}

		if (!EEq(turnNotches.Length2(), 0)) {
			const auto & item = ItemCursorRef->GetItemData();
			const auto yaw = item->Template.RotationInterval.Yaw;
			const auto pitch = item->Template.RotationInterval.Pitch;

			// Bind the notches, since the turn interval only accepts unsigned integers
			while (turnNotches.X < 0)
				turnNotches.X += yaw;
			while (turnNotches.X < 0)
				turnNotches.Y += pitch;

			turnNotches.X %= yaw;
			turnNotches.Y %= pitch;

			ItemRotation rot(-turnNotches.X, turnNotches.Y);
			ItemCursorRef->GetItemData()->AddRotation(rot);
		}
	}
}

void APlayerCharacter::MoveForward(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > FLOAT_ERROR) {
		FRotator rotator = Controller->GetControlRotation();

		if (CharacterMovement->IsMovingOnGround() || CharacterMovement->IsFalling())
			rotator.Pitch = 0.0;

		const FVector direction = FRotationMatrix(rotator).GetScaledAxis(EAxis::X);
		AddMovementInput(direction, amount);
	}
}

void APlayerCharacter::MoveRight(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > FLOAT_ERROR) {
		FRotator rotator = Controller->GetControlRotation();
		const FVector direction = FRotationMatrix(rotator).GetScaledAxis(EAxis::Y);
		AddMovementInput(direction, amount);
	}
}

void APlayerCharacter::LookUp(float amount) {
	if (bRotatingItem) {
		MouseHoldOffset.Y += amount / 10.0;
	} else {
		AddControllerPitchInput(amount);
	}
}

void APlayerCharacter::LookRight(float amount) {
	if (bRotatingItem) {
		MouseHoldOffset.X += amount / 10.0;
	} else {
		AddControllerYawInput(amount);
	}
}

void APlayerCharacter::HoldJump() {
	bHoldingJump = true;
	bPressedJump = true;
}

void APlayerCharacter::ReleaseJump() {
	bHoldingJump = false;
	bPressedJump = false;
}

void APlayerCharacter::OnRightMouseDown() {
	switch (CharDataRef->GetCurrentHandAction()) {
	case H_None:
		break;
	case H_Item:
		bRotatingItem = true;
		MouseHoldOffset.Reset(0, 0);
		break;
	case H_Tool:
		break;
	}
}

void APlayerCharacter::OnRightMouseUp() {
	if (bRotatingItem) {
		bRotatingItem = false;
		MouseHoldOffset.Reset(0, 0);
	} else {
		switch (CharDataRef->GetCurrentHandAction()) {
		case H_None: {
			// TODO: item interactions
			const auto viewpoint = GetViewRay();
			const auto foundResult =
				ChunkManagerRef->Raytrace(viewpoint, TerrainInteractionDistance);

			if (foundResult.IsValid()) {
				const auto & deref = *foundResult;
				if (deref.Type == E_PlacedItem) {
					auto itemRef = ChunkManagerRef->FindPlacedItem(*deref.ItemId);
					itemRef->Interact(this);
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

void APlayerCharacter::OnLeftMouseUp() {
	switch (CharDataRef->GetCurrentHandAction()) {
	case H_None:
		break;
	case H_Item:
		if (ItemCursorRef->IsValid() && !ItemCursorRef->IsHidden()) {
			const auto curItem = CharDataRef->GetCurrentItemInInventory();
			// TODO: if we wish to preserve item state, we'll need to place the original item data here
			if (curItem) {
				if (ChunkManagerRef->PlaceItem(ItemDataPtr(new ItemData(*curItem))))
					CharDataRef->PlaceCurrentItemInInventory();
			}
		}
		break;
	case H_Tool:
		break;
	}
}

void APlayerCharacter::HoldPrevItem() {
	CharDataRef->PrevHeldItem();
}

void APlayerCharacter::HoldNextItem() {
	CharDataRef->NextHeldItem();
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(
			-1, 5.0, FColor::Green, TEXT("Using standard game character."));

	auto gameState = GetWorld()->GetGameState<ADDGameState>();

	EventBusRef = gameState->EventBus;
	ChunkManagerRef = *(TActorIterator<AChunkManager>(GetWorld())); // There should always be a chunk manager
	TerrainParams = &gameState->ChunkLoader->GetGeneratorParameters();

	SetUpItemCursor();
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent * InputComponent) {
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	InputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	InputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRight);

	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::HoldJump);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::ReleaseJump);
	InputComponent->BindAction("RightMouseClick", IE_Pressed, this, &APlayerCharacter::OnRightMouseDown);
	InputComponent->BindAction("RightMouseClick", IE_Released, this, &APlayerCharacter::OnRightMouseUp);
	InputComponent->BindAction("LeftMouseClick", IE_Released, this, &APlayerCharacter::OnLeftMouseUp);
	InputComponent->BindAction("PrevItem", IE_Released, this, &APlayerCharacter::HoldPrevItem);
	InputComponent->BindAction("NextItem", IE_Released, this, &APlayerCharacter::HoldNextItem);
	InputComponent->BindAction("ToggleHandAction", IE_Released, this, &APlayerCharacter::ToggleHandAction);
}

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

		EventBusRef->BroadcastEvent(EventDataPtr(new EViewPosition(GetViewRay())));
	}

	// Tick once every half-second
	if (PositionSecondCount >= 0.5) {
		PositionSecondCount -= 0.5;
		
		EventBusRef->BroadcastEvent(
			EventDataPtr(new EPlayerPosition(ToVector3D(GetActorLocation()))));
	}
}

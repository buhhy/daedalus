#include "Daedalus.h"
#include "PlayerCharacter.h"

#include <Controllers/DDGameState.h>
#include <Utilities/Constants.h>
#include <Utilities/UnrealBridge.h>

using namespace utils;
using namespace events;
using namespace items;
using namespace terrain;

APlayerCharacter::APlayerCharacter(const class FPostConstructInitializeProperties& PCIP) :
	Super(PCIP), bHoldingJump(false), MouseHoldOffset(0, 0),
	PositionSecondCount(0), ViewSecondCount(0), TerrainInteractionDistance(250),
	ItemDataFactory(new items::ItemDataFactory())
{
	auto & movement = this->CharacterMovement;
	movement->SetWalkableFloorAngle(60.0);
	movement->JumpZVelocity = 400;
	movement->AirControl = 0.4;
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
	CurrentHeldItem = ItemDataFactory->BuildItemData(I_Sofa);
	ItemCursorRef = GetWorld()->SpawnActor<AItemCursor>(
		AItemCursor::StaticClass(), { 0, 0, 0 }, { 0, 0, 0 }, params);
	ItemCursorRef->Initialize(CurrentHeldItem);
	ItemCursorRef->SetActorEnableCollision(false);
	ItemCursorRef->SetActorHiddenInGame(true);
	ItemCursorRef->AttachRootComponentToActor(this);
}

void APlayerCharacter::UpdateItemCursor(const Ray3D & viewpoint) {
	if (CurrentHeldItem) {
		const auto foundResult =
			ChunkManagerRef->Raytrace(viewpoint, TerrainInteractionDistance);

		if (foundResult.Type == E_Terrain || foundResult.Type == E_PlacedItem) {
			ItemCursorRef->SetPlayerTransform(
				ToVector3D(this->GetActorLocation()), FRotationMatrix(this->GetActorRotation()));
			ItemCursorRef->SetPosition(foundResult.EntryPosition);
			ItemCursorRef->SetActorHiddenInGame(false);
		} else {
			ItemCursorRef->SetActorHiddenInGame(true);
		}
	}
}

void APlayerCharacter::UpdateItemCursorRotation() {
	if (CurrentHeldItem) {
		// Find out how many notches to turn
		Vector2D<Int16> turnNotches;
		for (Uint8 i = 0; i < 2; i++) {
			turnNotches[i] = (Int16) MouseHoldOffset[i];
			if (turnNotches[i] != 0)
				MouseHoldOffset[i] -= turnNotches[i];
		}

		if (!EEq(turnNotches.Length2(), 0)) {
			const auto yaw = CurrentHeldItem->Template.RotationInterval.Yaw;
			const auto pitch = CurrentHeldItem->Template.RotationInterval.Pitch;

			// Bind the notches, since the turn interval only accepts unsigned integers
			while (turnNotches.X < 0)
				turnNotches.X += yaw;
			while (turnNotches.X < 0)
				turnNotches.Y += pitch;

			turnNotches.X %= yaw;
			turnNotches.Y %= pitch;

			ItemRotation rot(turnNotches.X, turnNotches.Y);
			ItemCursorRef->AddRotation(rot);
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
		UpdateItemCursorRotation();
	} else {
		AddControllerPitchInput(amount);
	}
}

void APlayerCharacter::LookRight(float amount) {
	if (bRotatingItem) {
		MouseHoldOffset.X += amount / 10.0;
		UpdateItemCursorRotation();
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

void APlayerCharacter::BeginRotation() {
	// TODO: handle inventory logic
	bRotatingItem = true;
	MouseHoldOffset.Reset(0, 0);
	//EventBusRef->BroadcastEvent(EventDataPtr(new EFPItemPlacementBegin(GetViewRay())));
}

void APlayerCharacter::EndRotation() {
	bRotatingItem = false;
	MouseHoldOffset.Reset(0, 0);
	//EventBusRef->BroadcastEvent(EventDataPtr(new EFPItemPlacementEnd(GetViewRay())));
}

void APlayerCharacter::PlaceItem() {
	if (CurrentHeldItem)
		ChunkManagerRef->PlaceItem(ItemDataPtr(new ItemData(*CurrentHeldItem)));
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, TEXT("Using standard game character."));

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
	InputComponent->BindAction("RightMouseClick", IE_Pressed, this, &APlayerCharacter::BeginRotation);
	InputComponent->BindAction("RightMouseClick", IE_Released, this, &APlayerCharacter::EndRotation);
	InputComponent->BindAction("LeftMouseClick", IE_Released, this, &APlayerCharacter::PlaceItem);
}

void APlayerCharacter::Tick(float delta) {
	Super::Tick(delta);
	PositionSecondCount += delta;
	ViewSecondCount += delta;

	// Tick once every 20 milliseconds
	if (ViewSecondCount >= 0.02) {
		ViewSecondCount -= 0.02;
		
		UpdateItemCursor(GetViewRay());

		EventBusRef->BroadcastEvent(EventDataPtr(new EViewPosition(GetViewRay())));
	}

	// Tick once every half-second
	if (PositionSecondCount >= 0.5) {
		PositionSecondCount -= 0.5;
		
		EventBusRef->BroadcastEvent(
			EventDataPtr(new EPlayerPosition(ToVector3D(GetActorLocation()))));
	}
}

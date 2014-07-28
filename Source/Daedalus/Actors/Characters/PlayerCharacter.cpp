#include "Daedalus.h"
#include "PlayerCharacter.h"

#include <Controllers/DDGameState.h>
#include <Utilities/Constants.h>
#include <Utilities/UnrealBridge.h>

using namespace utils;
using namespace events;

APlayerCharacter::APlayerCharacter(const class FPostConstructInitializeProperties& PCIP) :
	Super(PCIP), bHoldingJump(false), MouseHoldOffset(0, 0),
	PositionSecondCount(0), ViewSecondCount(0)
{
	auto & movement = this->CharacterMovement;
	movement->SetWalkableFloorAngle(60.0);
	movement->JumpZVelocity = 400;
	movement->AirControl = 0.4;
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
	if (bPlacingItem)
		MouseHoldOffset.Y += amount;
	else
		AddControllerPitchInput(amount);
}

void APlayerCharacter::LookRight(float amount) {
	if (bPlacingItem)
		MouseHoldOffset.X += amount;
	else
		AddControllerYawInput(amount);
}

void APlayerCharacter::HoldJump() {
	bHoldingJump = true;
	bPressedJump = true;
}

void APlayerCharacter::ReleaseJump() {
	bHoldingJump = false;
	bPressedJump = false;
}

void APlayerCharacter::BeginPlaceItem() {
	// TODO: handle inventory logic
	bPlacingItem = true;
	MouseHoldOffset.Reset(0, 0);
	EventBusRef->BroadcastEvent(EventDataPtr(new EFPItemPlacementBegin()));
}

void APlayerCharacter::FinalizePlaceItem() {
	bPlacingItem = false;
	MouseHoldOffset.Reset(0, 0);
	EventBusRef->BroadcastEvent(EventDataPtr(new EFPItemPlacementEnd()));
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, TEXT("Using standard game character."));
	EventBusRef = GetWorld()->GetGameState<ADDGameState>()->EventBus;
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent * InputComponent) {
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	InputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	InputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRight);
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::HoldJump);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::ReleaseJump);
	InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &APlayerCharacter::BeginPlaceItem);
	InputComponent->BindAction("LeftMouseClick", IE_Released, this, &APlayerCharacter::FinalizePlaceItem);
}

void APlayerCharacter::Tick(float delta) {
	Super::Tick(delta);
	PositionSecondCount += delta;
	ViewSecondCount += delta;

	// Tick once every 20 milliseconds
	if (ViewSecondCount >= 0.02) {
		ViewSecondCount -= 0.02;

		FVector pos;
		FRotator dir;
		
		GetActorEyesViewPoint(pos, dir);
		EventBusRef->BroadcastEvent(
			EventDataPtr(new EViewPosition(ToVector3D(pos), ToVector3D(dir.Vector()))));

		if (bPlacingItem) {
			EventBusRef->BroadcastEvent(
				EventDataPtr(new EFPItemPlacementRotation(MouseHoldOffset)));
		}
	}

	// Tick once every half-second
	if (PositionSecondCount >= 0.5) {
		PositionSecondCount -= 0.5;
		
		EventBusRef->BroadcastEvent(
			EventDataPtr(new EPlayerPosition(ToVector3D(GetActorLocation()))));
	}
}

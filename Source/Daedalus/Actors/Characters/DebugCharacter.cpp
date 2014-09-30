#include "Daedalus.h"
#include "DebugCharacter.h"

#include <Controllers/DDGameState.h>
#include <Utilities/Constants.h>

#include <Utilities/UnrealBridge.h>

ADebugCharacter::ADebugCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), TickDeltaCount(0)
{
	auto & movement = this->CharacterMovement;
	movement->SetWalkableFloorAngle(60.0);
	movement->BrakingDecelerationFlying = 1000.0;
	movement->DefaultLandMovementMode = MOVE_Flying;
}

void ADebugCharacter::MoveUp(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > utils::FLOAT_ERROR) {
		const FVector direction(0, 0, 1.0);
		AddMovementInput(direction, amount);
	}
}

void ADebugCharacter::BeginPlay() {
	Super::BeginPlay();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, TEXT("Using debug game character."));
}

void ADebugCharacter::SetupPlayerInputComponent(class UInputComponent * InputComponent) {
	//InputComponent->BindAxis("MoveForward", this, &ADebugCharacter::MoveForward);
	//InputComponent->BindAxis("MoveRight", this, &ADebugCharacter::MoveRight);
	//InputComponent->BindAxis("MoveUp", this, &ADebugCharacter::MoveUp);
	//InputComponent->BindAxis("LookUp", this, &ADebugCharacter::LookUp);
	//InputComponent->BindAxis("LookRight", this, &ADebugCharacter::LookRight);
}



/********************************************************************************
 * Implementation for DDPlayerController
 ********************************************************************************/

void ADebugCharacter::moveForward(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > utils::FLOAT_ERROR) {
		FRotator rotator = Controller->GetControlRotation();
		rotator.Pitch = 0.0;

		const FVector direction = FRotationMatrix(rotator).GetScaledAxis(EAxis::X);
		AddMovementInput(direction, amount);
	}
}

void ADebugCharacter::moveRight(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > utils::FLOAT_ERROR) {
		FRotator rotator = Controller->GetControlRotation();
		const FVector direction = FRotationMatrix(rotator).GetScaledAxis(EAxis::Y);
		AddMovementInput(direction, amount);
	}
}

void ADebugCharacter::lookUp(float amount) {
	AddControllerPitchInput(amount);
}

void ADebugCharacter::lookRight(float amount) {
	AddControllerYawInput(amount);
}

void ADebugCharacter::holdJump() {}
void ADebugCharacter::releaseJump() {}
void ADebugCharacter::rightMouseDown() {}
void ADebugCharacter::rightMouseUp() {}
void ADebugCharacter::leftMouseDown() {}
void ADebugCharacter::leftMouseUp() {}
void ADebugCharacter::escapeKey() {}
void ADebugCharacter::toggleHandAction() {}

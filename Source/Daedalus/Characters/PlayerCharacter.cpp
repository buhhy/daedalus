#include "Daedalus.h"
#include "PlayerCharacter.h"
#include "Constants.h"

APlayerCharacter::APlayerCharacter(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP), bHoldingJump(false) {
	auto & movement = this->CharacterMovement;
	movement->SetWalkableFloorAngle(60.0);
	movement->JumpZVelocity = 400;
	movement->AirControl = 0.4;
}

void APlayerCharacter::MoveForward(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > ERROR) {
		FRotator rotator = Controller->GetControlRotation();

		if (CharacterMovement->IsMovingOnGround() || CharacterMovement->IsFalling())
			rotator.Pitch = 0.0;

		const FVector direction = FRotationMatrix(rotator).GetScaledAxis(EAxis::X);
		AddMovementInput(direction, amount);
	}
}

void APlayerCharacter::MoveRight(float amount) {
	if (Controller != NULL && FMath::Abs(amount) > ERROR) {
		FRotator rotator = Controller->GetControlRotation();
		const FVector direction = FRotationMatrix(rotator).GetScaledAxis(EAxis::Y);
		AddMovementInput(direction, amount);
	}
}

void APlayerCharacter::LookUp(float amount) {
	AddControllerPitchInput(amount);
}

void APlayerCharacter::LookRight(float amount) {
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

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, TEXT("USING CHARACTER!"));
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent * InputComponent) {
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	InputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	InputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRight);
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::HoldJump);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::ReleaseJump);
}

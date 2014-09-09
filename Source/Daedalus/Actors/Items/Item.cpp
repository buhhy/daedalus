#include <Daedalus.h>
#include "Item.h"

#include <sstream>

#include <Actors/Characters/PlayerCharacter.h>
#include <Controllers/DDGameState.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Utilities/UnrealBridge.h>

using namespace items;
using namespace utils;
using namespace terrain;

AItem::AItem(const class FPostConstructInitializeProperties & PCIP) :
	Super(PCIP), tickCount(0), lastRotation(0, 0), lastPosition()
{
	MeshComponent = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Mesh"));
	MeshComponent->SetMobility(EComponentMobility::Movable);
	this->RootComponent = MeshComponent;
	this->PrimaryActorTick.bCanEverTick = true;
}

void AItem::AssertInitialized() const {
	assert(ItemData && "AItem::AssertInitialized: Class has not been initialized");
}

void AItem::SetRelativeTransform(const FVector & location, const FRotator & rot) {
	this->MeshComponent->SetRelativeLocationAndRotation(location, rot);
}

void AItem::applyTransform() {
	AssertInitialized();
	if (lastPosition != ItemData->getPosition() || lastRotation != ItemData->getRotation()) {
		lastRotation = ItemData->getRotation();
		lastPosition = ItemData->getPosition();

		const auto transform = ItemData->GetPositionMatrix();
		Basis3D basis = GetBasisFrom(transform);
		auto fRotMat = FRotationMatrix::MakeFromXZ(
			ToFVector(basis.XVector), ToFVector(basis.ZVector));
		const auto trans = TerrainParams->ToRealCoordSpace(
			GetTranslationVectorFrom(transform));
		SetRelativeTransform(ToFVector(trans), fRotMat.Rotator());
	}
}

void AItem::applyScale() {
	AssertInitialized();
	if (lastScale != ItemData->getScale()) {
		lastScale = ItemData->getScale();
		SetActorRelativeScale3D(ToFVector(lastScale));
	}
}

void AItem::LoadMesh(const std::string & meshName) {
	std::stringstream smn;
	smn << "SkeletalMesh'/Game/" << meshName << "." << meshName << "_" << meshName << "'";
	MeshComponent->SetSkeletalMesh(FindSkeletalMesh(smn.str()));
	smn.str(""); smn.clear();
	smn << "AnimBlueprint'/Game/Animation" << meshName << ".Animation" << meshName << "'";
	auto anim = FindAnimBlueprint(smn.str());
	if (anim)
		MeshComponent->SetAnimClass(anim->GetAnimBlueprintGeneratedClass());
}

bool AItem::IsItemInUse() {
	return ItemData && ItemData->isInUse();
}

void AItem::BeginPlay() {
	if (TerrainParams == NULL)
		TerrainParams =
			&GetWorld()->GetGameState<ADDGameState>()->ChunkLoader->GetGeneratorParameters();
	Super::BeginPlay();
}

void AItem::initialize(const ItemDataPtr & data) {
	ItemData = data;
	LoadMesh(data->Template.MeshName);
	applyTransform();
	applyScale();
}

//void AItem::Interact(APlayerCharacter * player) {
//	auto str = "Interacting with " + ItemData->Template.MeshName;
//	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, UTF8_TO_TCHAR(str.c_str()));
//}

void AItem::Tick(float interval) {
	if (ItemData) {
		tickCount += interval;

		if (tickCount >= ItemData->Template.tickDuration) {
			tickCount -= ItemData->Template.tickDuration;
		}
		
		applyTransform();
		applyScale();
	}
}

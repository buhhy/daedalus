#include <Daedalus.h>
#include "Item.h"

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
	MeshComponent = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	MeshComponent->SetMobility(EComponentMobility::Movable);
	this->RootComponent = MeshComponent;
}

void AItem::AssertInitialized() const {
	assert(ItemData != NULL && "AItem::AssertInitialized: Class has not been initialized");
}

void AItem::SetRelativeTransform(const FVector & location, const FRotator & rot) {
	this->MeshComponent->SetRelativeLocationAndRotation(location, rot);
}

void AItem::ApplyTransform() {
	AssertInitialized();
	if (lastPosition != ItemData->getPosition() || lastRotation != ItemData->getRotation()) {
		lastRotation = ItemData->getRotation();
		lastPosition = ItemData->getPosition();

		const auto transform = ItemData->GetPositionMatrix();
		Basis3D basis = GetBasisFrom(transform);
		auto fRotMat = FRotationMatrix::MakeFromXZ(ToFVector(basis.XVector), ToFVector(basis.ZVector));
		const auto trans = TerrainParams->ToRealCoordSpace(GetTranslationVectorFrom(transform));
		SetRelativeTransform(ToFVector(trans), fRotMat.Rotator());
	}
}

void AItem::LoadMesh(const std::string & meshName) {
	auto path = "StaticMesh'/Game/" + meshName + "'";
	MeshComponent->SetStaticMesh(FindStaticMesh(path));
}

void AItem::BeginPlay() {
	if (TerrainParams == NULL)
		TerrainParams =
			&GetWorld()->GetGameState<ADDGameState>()->ChunkLoader->GetGeneratorParameters();
	Super::BeginPlay();
}

void AItem::Initialize(const ItemDataPtr & data) {
	ItemData = data;
	LoadMesh(data->Template.MeshName);
	ApplyTransform();
}

void AItem::Interact(APlayerCharacter * player) {
	auto str = "Interacting with " + ItemData->Template.MeshName;
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, UTF8_TO_TCHAR(str.c_str()));
}

void AItem::Tick(float interval) {
	if (ItemData) {
		tickCount += interval;

		if (tickCount >= ItemData->Template.tickDuration) {
			tickCount -= ItemData->Template.tickDuration;
			ApplyTransform();
		}
	}
}

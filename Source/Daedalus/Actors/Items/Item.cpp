#include <Daedalus.h>
#include "Item.h"

#include <Controllers/DDGameState.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Utilities/UnrealBridge.h>

using namespace items;
using namespace utils;

AItem::AItem(const class FPostConstructInitializeProperties & PCIP) : Super(PCIP) {
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
	const auto transform = ItemData->GetPositionMatrix();
	Basis3D basis = GetBasisFrom(transform);
	auto fRotMat = FRotationMatrix::MakeFromXZ(ToFVector(basis.XVector), ToFVector(basis.ZVector));
	const auto trans = TerrainParams->ToRealCoordSpace(GetTranslationVectorFrom(transform));
	SetRelativeTransform(ToFVector(trans), fRotMat.Rotator());
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

void AItem::SetPosition(const terrain::ChunkPositionVector & position) {
	AssertInitialized();
	ItemData->Position = TerrainParams->Normalize(position);
	ApplyTransform();
}

void AItem::SetRotation(const ItemRotation & rotation) {
	AssertInitialized();
	ItemData->SetRotation(rotation);
	ApplyTransform();
}

void AItem::AddRotation(const ItemRotation & rotation) {
	AssertInitialized();
	ItemData->AddRotation(rotation);
	ApplyTransform();
}

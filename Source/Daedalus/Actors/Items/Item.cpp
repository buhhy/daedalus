#include <Daedalus.h>
#include "Item.h"

#include <Controllers/DDGameState.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Utilities/UnrealBridge.h>

using namespace items;
using namespace utils;

AItem::AItem(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP)
{
	MeshComponent = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMesh"));

	//MeshComponent->SetStaticMesh(GetMesh());
}

void AItem::AssertInitialized() const {
	assert(ItemData != NULL && "AItem::AssertInitialized: Class has not been initialized");
}

void AItem::ApplyTransform() {
	const auto transform = ItemData->GetPositionMatrix();
	Basis3D basis = GetBasisFrom(transform);
	auto fRotMat = FRotationMatrix::MakeFromXZ(ToFVector(basis.XVector), ToFVector(basis.ZVector));
	const auto trans = GenParams->ToRealChunkCoordSpace(GetTranslationVectorFrom(transform));
	MeshComponent->SetRelativeLocationAndRotation(ToFVector(trans), fRotMat.Rotator());
}

void AItem::LoadMesh(const std::string & meshName) {
	auto path = "StaticMesh'/Game/" + meshName + "'";
	MeshComponent->SetStaticMesh(FindStaticMesh(*FString(path.c_str())));
	MeshComponent->SetMobility(EComponentMobility::Movable);
	this->RootComponent = MeshComponent;
}

void AItem::Initialize(const ItemDataPtr & data) {
	GenParams = &GetWorld()->GetGameState<ADDGameState>()->ChunkLoader->GetGeneratorParameters();;
	ItemData = data;
	LoadMesh(data->Template.MeshName);
	ApplyTransform();
}

void AItem::SetPosition(const terrain::ChunkPositionVector & position) {
	AssertInitialized();
	ItemData->Position = position;
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

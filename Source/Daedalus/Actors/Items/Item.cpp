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

void AItem::AdjustRotationMatrix() {
	AssertInitialized();
	RotationMatrix = ItemData->GetRotationMatrix();
}

void AItem::AdjustPositionMatrix() {
	AssertInitialized();
	TranslationVector = ItemData->Position.second;
}

void AItem::ApplyTransform() {
	Vector3D<> x, y, z;
	RotationMatrix.GetBasis(x, y, z);
	auto fRotMat = FRotationMatrix::MakeFromXZ(ToFVector(x), ToFVector(z));
	const auto trans = GenParams->ToRealInnerCoordinates(TranslationVector +
		RotationMatrix.GetTranslationVector());
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
	AdjustPositionMatrix();
	AdjustRotationMatrix();
	ApplyTransform();
}

void AItem::SetPosition(const terrain::ChunkPositionVector & position) {
	AssertInitialized();
	ItemData->Position = position;
	AdjustPositionMatrix();
	ApplyTransform();
}

void AItem::SetRotation(const ItemRotation & rotation) {
	AssertInitialized();
	ItemData->SetRotation(rotation);
	AdjustRotationMatrix();
	ApplyTransform();
}

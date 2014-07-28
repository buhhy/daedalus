#include <Daedalus.h>
#include "Item.h"

using namespace items;

AItem::AItem(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP)
{
	MeshComponent = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMesh"));

	//MeshComponent->SetStaticMesh(GetMesh());
}

void AItem::PostInitialize() {
	MeshComponent->SetMobility(EComponentMobility::Movable);
	this->RootComponent = MeshComponent;
}

void AItem::SetItemData(const ItemDataPtr & data) {
	ItemData = data;
}

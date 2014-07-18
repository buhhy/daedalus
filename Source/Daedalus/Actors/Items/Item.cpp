#include <Daedalus.h>
#include "Item.h"

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

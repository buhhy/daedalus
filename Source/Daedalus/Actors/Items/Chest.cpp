#include <Daedalus.h>
#include "Chest.h"


AChest::AChest(const FPostConstructInitializeProperties & PCIP)
	: Super(PCIP)
{
	MeshComponent->SetStaticMesh(FindStaticMesh(TEXT("StaticMesh'/Game/Chest.Chest'")));
	PostInitialize();
}


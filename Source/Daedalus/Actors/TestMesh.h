#pragma once

#include "GameFramework/Actor.h"

#include <Actors/CustomComponents/GeneratedMeshComponent.h>

#include "TestMesh.generated.h"

/**
* In-game actor that renders the chunk and the collision mesh.
*/
UCLASS()
class ATestMesh : public AActor {
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Mesh")
		TSubobjectPtr<UGeneratedMeshComponent> Mesh;
};

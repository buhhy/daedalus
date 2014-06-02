#pragma once

#include "GameFramework/Actor.h"
#include "GeneratedMeshComponent.h"
#include "ChunkData.h"
#include "DataStructures.h"
#include "Chunk.generated.h"

/**
* In-game actor that renders the chunk and the collision mesh.
*/
UCLASS()
class AChunk : public AActor {
	GENERATED_UCLASS_BODY()

private:
	ChunkData ChunkData;
	void TestRender();

public:
	UPROPERTY()
		TSubobjectPtr<UGeneratedMeshComponent> Mesh;

	void InitializeChunk(
		const utils::Vector3<uint64> & chunkSize,
		const utils::Vector3<int64> & chunkOffset,
		uint64 seed);
};


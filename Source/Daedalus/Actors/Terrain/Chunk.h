#pragma once

#include "GameFramework/Actor.h"
#include "GeneratedMeshComponent.h"
#include "ChunkData.h"
#include "TerrainDataStructures.h"

#include <Utilities/Algebra/Algebra3D.h>

#include "Chunk.generated.h"

/**
* In-game actor that renders the chunk and the collision mesh.
*/
UCLASS()
class AChunk : public AActor {
	GENERATED_UCLASS_BODY()

private:
	std::shared_ptr<terrain::ChunkData> ChunkData;
	terrain::TerrainGeneratorParameters TerrainGenParams;
	void GenerateChunkMesh();

public:
	UPROPERTY()
		TSubobjectPtr<UGeneratedMeshComponent> Mesh;
	UPROPERTY()
		UMaterial * TestMaterial;

	void InitializeChunk(const terrain::TerrainGeneratorParameters & params);
	void SetChunkData(const std::shared_ptr<terrain::ChunkData> & chunkData);
};

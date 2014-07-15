#pragma once

#include "GameFramework/Actor.h"
#include "GeneratedMeshComponent.h"

#include <Models/Terrain/ChunkData.h>
#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>

#include "Chunk.generated.h"

/**
* In-game actor that renders the chunk and the collision mesh.
*/
UCLASS()
class AChunk : public AActor {
	GENERATED_UCLASS_BODY()
public:
	// (0, 0) is the root chunk data for this actor, the others are neighbours for tiling
	using ChunkDataSet = utils::TensorFixed3D<terrain::ChunkDataPtr, 2>;

private:
	ChunkDataSet ChunkData;
	terrain::TerrainGeneratorParameters TerrainGenParams;
	void GenerateChunkMesh();

public:
	UPROPERTY()
		TSubobjectPtr<UGeneratedMeshComponent> Mesh;
	UPROPERTY()
		UMaterial * TestMaterial;

	void InitializeChunk(const terrain::TerrainGeneratorParameters & params);
	void SetChunkData(const ChunkDataSet & chunkData);
};

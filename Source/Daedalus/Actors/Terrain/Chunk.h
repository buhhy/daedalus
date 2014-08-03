#pragma once

#include "GameFramework/Actor.h"
#include "GeneratedMeshComponent.h"

#include <Actors/Items/Item.h>
#include <Controllers/DDGameState.h>
#include <Models/Terrain/ChunkData.h>
#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Utilities/FastVoxelTraversal.h>

#include <utility>

#include "Chunk.generated.h"

USTRUCT()
struct FItemPtrPair {
	GENERATED_USTRUCT_BODY()
public:
	Uint64 ItemId;
	UPROPERTY(Category = Item, VisibleAnywhere)
		AItem * ItemActor;
};

/**
* In-game actor that renders the chunk and the collision mesh.
*/
UCLASS()
class AChunk : public AActor {
	GENERATED_UCLASS_BODY()
public:
	// (1, 1) is the root chunk data for this actor, the others are neighbours for tiling
	using ChunkDataSet = utils::TensorFixed3D<terrain::ChunkDataPtr, 3>;

private:
	ChunkDataSet ChunkNeighbourData;
	terrain::ChunkDataPtr CurrentChunkData;
	const terrain::TerrainGeneratorParameters * TerrainGenParams;
	utils::TensorResizable3D<bool> SolidTerrain;
	Uint64 ItemIdCounter;                            // Used to store the minimum unique ID


	bool IsOccupiedAt(const terrain::ChunkGridIndexVector & gridIndex) const;
	void GenerateChunkMesh();
	AItem * SpawnItem(const items::ItemDataPtr & itemData);
	items::ItemDataPtr RemoveItem(const items::ItemDataPtr & itemData);

protected:
	virtual void ReceiveDestroyed() override;

public:
	UPROPERTY(Category = Mesh, VisibleAnywhere)
		TSubobjectPtr<UGeneratedMeshComponent> Mesh;
	UPROPERTY()
		UMaterial * TestMaterial;
	UPROPERTY(Category = Items, VisibleAnywhere)
		TArray<FItemPtrPair> PlacedItems;

	void InitializeChunk(const terrain::TerrainGeneratorParameters * params);
	void SetChunkData(const ChunkDataSet & chunkData);
	AItem * CreateItem(const items::ItemDataPtr & itemData, const bool preserveId = false);
	
	/**
	 * @param ray The origin of the ray should be in [0, 1] chunk inner coordinates, the
	 *            direction should be normal.
	 * @param maxDistance Maximum number of grid cells to search for solid blocks.
	 */
	bool TerrainIntersection(
		utils::Vector3D<Int64> & collisionIndex,
		utils::Vector3D<Int64> & precollisionIndex,
		const utils::Ray3D & ray,
		const double maxDistance) const;
};

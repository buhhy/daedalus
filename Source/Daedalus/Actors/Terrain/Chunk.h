#pragma once

#include "GameFramework/Actor.h"
#include "GeneratedMeshComponent.h"

#include <Actors/Items/ItemFactory.h>
#include <Controllers/DDGameState.h>
#include <Models/Terrain/ChunkData.h>
#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <utility>

#include "Chunk.generated.h"

USTRUCT()
struct FItemPtrPair {
	GENERATED_USTRUCT_BODY()
public:
	item::ItemDataId ItemId;
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
	// (0, 0) is the root chunk data for this actor, the others are neighbours for tiling
	using ChunkDataSet = utils::TensorFixed3D<terrain::ChunkDataPtr, 2>;

private:
	ChunkDataSet ChunkNeighbourData;
	terrain::ChunkDataPtr CurrentChunkData;
	terrain::TerrainGeneratorParameters TerrainGenParams;
	utils::TensorResizable3D<bool> SolidTerrain;

	const UItemFactory * ItemFactory;


	inline ADDGameState * GetGameState() { return GetWorld()->GetGameState<ADDGameState>(); }
	void GenerateChunkMesh();
	void SpawnItem(const item::ItemDataPtr & itemData);
	void RemoveItem(const item::ItemDataPtr & itemData);

protected:
	virtual void ReceiveDestroyed() override;

public:
	UPROPERTY(Category = Mesh, VisibleAnywhere)
		TSubobjectPtr<UGeneratedMeshComponent> Mesh;
	UPROPERTY()
		UMaterial * TestMaterial;
	UPROPERTY(Category = Items, VisibleAnywhere)
		TArray<FItemPtrPair> PlacedItems;

	void InitializeChunk(
		const terrain::TerrainGeneratorParameters & params,
		const UItemFactory * itemFactory);
	void SetChunkData(const ChunkDataSet & chunkData);
	
	/**
	 * @param ray The origin of the ray should be in chunk grid coordinate space, and the
	 *            direction vector should be a normal vector
	 * @param maxDistance Maximum number of grid cells to search for solid blocks.
	 */
	bool SolidIntersection(
		terrain::ChunkGridIndexVector & result,
		const utils::Ray3D & ray, const double maxDistance) const;
};

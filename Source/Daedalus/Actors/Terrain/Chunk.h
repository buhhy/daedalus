#pragma once

#include "GameFramework/Actor.h"

#include <Actors/CustomComponents/GeneratedMeshComponent.h>
#include <Actors/Items/Item.h>
#include <Controllers/DDGameState.h>
#include <Models/Terrain/ChunkData.h>
#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/DataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Utilities/FastVoxelTraversal.h>

#include <utility>

#include "Chunk.generated.h"

class AChunk;

namespace terrain {
	enum TerrainRaytraceResultType {
		E_Character,
		E_Terrain,
		E_PlacedItem
	};

	struct TerrainRaytraceResult {
		const TerrainRaytraceResultType Type;
		const items::ItemDataPtr ItemData;
		const ChunkPositionVector EntryPosition;

		TerrainRaytraceResult(
			const TerrainRaytraceResultType type,
			const ChunkPositionVector & pos,
			const items::ItemDataPtr & itemId
		) : Type(type), ItemData(itemId), EntryPosition(pos)
		{}
	};

	utils::Option<TerrainRaytraceResult> TerrainResult(const ChunkPositionVector & entry);
	utils::Option<TerrainRaytraceResult> ItemResult(
		const items::ItemDataPtr & id, const ChunkPositionVector & pos);
	utils::Option<TerrainRaytraceResult> NoResult();
}

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
	// Convenience vector that stores the position of the current chunk within the neighbour data
	utils::Vector3D<Uint32> CurrentChunkIndex;
	ChunkDataSet ChunkNeighbourData;
	terrain::ChunkDataPtr CurrentChunkData;

	const terrain::TerrainGeneratorParameters * TerrainGenParams;
	utils::TensorResizable3D<bool> SolidTerrain;
	Uint64 ItemIdCounter;                            // Used to store the minimum unique ID



	/**
	 * @param boundingBox Bounding box to search for intersections, the values should be in
	 *                    grid coordinate space.
	 */
	void GenerateChunkMesh();
	AItem * SpawnItem(const items::ItemDataPtr & itemData);
	items::ItemDataPtr RemoveItem(const items::ItemDataPtr & itemData);

	bool IsSolidTerrainAt(const utils::Point3D & point) const;
	bool IsSolidTerrainAt(const utils::AxisAlignedBoundingBox3D & bound) const;
	items::ItemDataPtr FindItemCollision(const utils::AxisAlignedBoundingBox3D & bound) const;
	/**
	 * Checks if the space specified by the bounding box is occupied or not. This is used for
	 * determining the validity of item placement.
	 */
	bool IsSpaceOccupied(const utils::AxisAlignedBoundingBox3D & bound) const;

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
	AItem * FindPlacedItem(const Uint64 uid);
	
	/**
	 * @param ray The origin of the ray should be in chunk inner coordinates, the
	 *            direction should be normal.
	 * @param maxDistance Maximum number of grid cells to search for solid blocks.
	 */
	utils::Option<terrain::TerrainRaytraceResult> Raytrace(
		const utils::Ray3D & ray, const double maxDistance);
};

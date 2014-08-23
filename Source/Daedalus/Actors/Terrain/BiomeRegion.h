#pragma once

#include "GameFramework/Actor.h"

#include <Actors/CustomComponents/GeneratedMeshComponent.h>
#include <Models/Terrain/BiomeRegionData.h>
#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <memory>

#include "BiomeRegion.generated.h"

/**
 * In-game actor that visualizes the biome generation algorithm. This is more of a debug actor
 * than anything since players are not supposed to be able to see this.
 */
UCLASS()
class ABiomeRegion : public AActor {
	GENERATED_UCLASS_BODY()

private:
	terrain::BiomeRegionDataPtr RegionData;
	float BiomeGridScale;
	float RenderHeight;
	void GenerateBiomeRegionMesh();

public:
	UPROPERTY()
		TSubobjectPtr<UGeneratedMeshComponent> Mesh;
	UPROPERTY()
		UMaterial * TestMaterial;

	void InitializeBiomeRegion(const float scale);
	void SetBiomeRegionData(const terrain::BiomeRegionDataPtr & BiomeRegionData);
};

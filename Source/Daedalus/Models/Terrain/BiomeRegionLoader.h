#pragma once

#include <unordered_map>
#include "BiomeRegionData.h"
#include "TerrainDataStructures.h"
#include "DataStructures.h"
#include "EventBus.h"

namespace terrain {
	typedef std::unordered_map<
		terrain::BiomeOffsetVector,
		TSharedRef<BiomeRegionData>
	> BiomeRegionCache;

	/**
	 * This class will load data related to a particular biome region. Biome regions will
	 * contain multiple biomes linked through a delaunay triangulation. After the generation
	 * of the triangulation and corresponding Voronoi diagram, we can generate specific
	 * features, allowing us to create more contextual terrain features like coherent mountain
	 * ranges and what not. This class will most likely run on the server-side.
	 */
	class BiomeRegionLoader {
	private:
		BiomeRegionCache LoadedBiomeRegionCache;

		BiomeGeneratorParameters BiomeGenParams;
		TSharedRef<events::EventBus> EventBus;


		TSharedPtr<BiomeRegionData> LoadBiomeRegionFromDisk(
			const BiomeOffsetVector & offset) const;
		TSharedRef<BiomeRegionData> GenerateMissingBiomeRegion(
			const BiomeOffsetVector & offset) const;

		bool MergeRegionEdge(BiomeRegionData & r1, BiomeRegionData & r2);
		bool MergeRegionCorner(
			const BiomeRegionData & tl,
			const BiomeRegionData & tr,
			const BiomeRegionData & bl,
			const BiomeRegionData & br);
		std::vector<BiomeOffsetVector> MergeRegion(
			TSharedRef<BiomeRegionData> targetRegion, const BiomeOffsetVector & biomeOffset);

	public:
		BiomeRegionLoader(
			const BiomeGeneratorParameters & params,
			TSharedRef<events::EventBus> eventBus);
		~BiomeRegionLoader();

		const BiomeGeneratorParameters & GetGeneratorParameters() const;
		TSharedRef<BiomeRegionData> GetBiomeRegionAt(const BiomeOffsetVector & offset);
	};
}

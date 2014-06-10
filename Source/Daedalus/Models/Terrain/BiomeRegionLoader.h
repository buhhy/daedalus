#pragma once

#include <unordered_map>
#include "BiomeRegionData.h"
#include "TerrainDataStructures.h"
#include "DataStructures.h"

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

		TSharedPtr<BiomeRegionData> LoadBiomeRegionFromDisk(const BiomeOffsetVector & offset);
		TSharedRef<BiomeRegionData> GenerateMissingBiomeRegion(const BiomeOffsetVector & offset);

	public:
		BiomeRegionLoader(const BiomeGeneratorParameters & params);
		~BiomeRegionLoader();

		const BiomeGeneratorParameters & GetGeneratorParameters() const;
		TSharedRef<BiomeRegionData> GetChunkAt(const BiomeOffsetVector & offset);
	};
}

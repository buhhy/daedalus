#pragma once

#include "BiomeRegionData.h"
#include "TerrainDataStructures.h"
#include <Utilities/DataStructures.h>
#include <Controllers/EventBus/EventBus.h>

#include <unordered_map>
#include <memory>

namespace terrain {
	typedef std::unordered_map<
		terrain::BiomeRegionOffsetVector,
		std::shared_ptr<BiomeRegionData>
	> BiomeRegionCache;

	/**
	 * This class will load data related to a particular biome region. Biome regions will
	 * contain multiple biomes linked through a delaunay triangulation. After the generation
	 * of the triangulation and corresponding Voronoi diagram, we can generate specific
	 * features, allowing us to create more contextual terrain features like coherent mountain
	 * ranges and what not. This class will most likely run on the server-side.
	 */
	class BiomeRegionLoader {
	public:
		typedef std::pair<utils::Vector2<>, uint32_t> VertexWithHullIndex;

	private:
		BiomeRegionCache LoadedBiomeRegionCache;

		BiomeGeneratorParameters BiomeGenParams;
		std::shared_ptr<events::EventBus> EventBus;

		std::shared_ptr<const VertexWithHullIndex> GetCornerHullVertex(
			const BiomeRegionData & data, const bool cornerX, const bool cornerY) const;
		
		bool IsBiomeRegionGenerated(const BiomeRegionOffsetVector & offset) const;


		std::shared_ptr<BiomeRegionData> LoadBiomeRegionFromDisk(
			const BiomeRegionOffsetVector & offset);
		std::shared_ptr<BiomeRegionData> GenerateBiomeRegion(
			const BiomeRegionOffsetVector & offset);
		std::shared_ptr<BiomeRegionData> GenerateBiomeRegionArea(
			const BiomeRegionOffsetVector & offset, const uint8_t radius);

		bool MergeRegionEdge(BiomeRegionData & r1, BiomeRegionData & r2);
		bool MergeRegionCorner(
			BiomeRegionData & tl,
			BiomeRegionData & tr,
			BiomeRegionData & bl,
			BiomeRegionData & br);
		std::unordered_set<BiomeRegionOffsetVector> MergeRegion(
			std::shared_ptr<BiomeRegionData> targetRegion);

	public:
		BiomeRegionLoader(
			const BiomeGeneratorParameters & params,
			std::shared_ptr<events::EventBus> eventBus);
		~BiomeRegionLoader();
		

		const BiomeGeneratorParameters & GetGeneratorParameters() const {
			return BiomeGenParams;
		}

		std::shared_ptr<BiomeRegionData> GetBiomeRegionAt(const BiomeRegionOffsetVector & offset);
		const BiomeId FindNearestBiomeId(const utils::Vector2<> point);
	};
}

#pragma once

#include "BiomeRegionData.h"
#include "TerrainDataStructures.h"
#include <Utilities/DataStructures.h>
#include <Controllers/EventBus/EventBus.h>

#include <unordered_map>
#include <memory>

namespace terrain {
	/**
	 * This class will load data related to a particular biome region. Biome regions will
	 * contain multiple biomes linked through a delaunay triangulation. After the generation
	 * of the triangulation and corresponding Voronoi diagram, we can generate specific
	 * features, allowing us to create more contextual terrain features like coherent mountain
	 * ranges and what not. This class will most likely run on the server-side.
	 */
	class BiomeRegionLoader {
	public:
		using VertexWithHullIndex = std::pair<utils::Vector2<>, Uint32>;
		using BiomeRegionDataPtr = std::shared_ptr<BiomeRegionData>;
		using DelaunayBuilderPtr = std::shared_ptr<utils::DelaunayBuilderDAC2D>;
		using UpdatedRegionSet = std::unordered_set<BiomeRegionOffsetVector>;
		using BiomeRegionCache =
			std::unordered_map<terrain::BiomeRegionOffsetVector, BiomeRegionDataPtr>;

	private:
		BiomeRegionCache LoadedBiomeRegionCache;
		BiomeGeneratorParameters BiomeGenParams;
		
		DelaunayBuilderPtr DelaunayBuilder;
		Uint8 FetchRadius;
		std::shared_ptr<events::EventBus> EventBus;
		std::shared_ptr<const VertexWithHullIndex> GetCornerHullVertex(
			const BiomeRegionData & data, const bool cornerX, const bool cornerY) const;



		
		bool IsBiomeRegionGenerated(const BiomeRegionOffsetVector & offset) const;
		BiomeRegionDataPtr GetBiomeRegionFromCache(const BiomeRegionOffsetVector & offset);

		BiomeRegionDataPtr LoadBiomeRegionFromDisk(
			const BiomeRegionOffsetVector & offset);
		BiomeRegionDataPtr GenerateBiomeRegion(
			const BiomeRegionOffsetVector & offset);
		BiomeRegionDataPtr GenerateBiomeRegionArea(
			UpdatedRegionSet & updatedRegions,
			const BiomeRegionOffsetVector & offset,
			const Uint8 radius);

		BiomeRegionDataPtr GenerateBiomeDataForRegion(
			UpdatedRegionSet & updatedRegions,
			BiomeRegionDataPtr biomeRegion);

		bool MergeRegionEdge(BiomeRegionData & r1, BiomeRegionData & r2);
		bool MergeRegionCorner(
			BiomeRegionData & tl,
			BiomeRegionData & tr,
			BiomeRegionData & bl,
			BiomeRegionData & br);
		void MergeRegion(
			UpdatedRegionSet & updatedRegions,
			BiomeRegionDataPtr targetRegion);

	public:
		BiomeRegionLoader(
			const BiomeGeneratorParameters & params,
			std::shared_ptr<events::EventBus> eventBus,
			DelaunayBuilderPtr builder =
				DelaunayBuilderPtr(new utils::DelaunayBuilderDAC2D()),
			Uint8 fetchRadius = 1);
		~BiomeRegionLoader();
		

		const BiomeGeneratorParameters & GetGeneratorParameters() const {
			return BiomeGenParams;
		}

		BiomeRegionDataPtr GetBiomeRegionAt(const BiomeRegionOffsetVector & offset);
		const BiomeId FindNearestBiomeId(const utils::Vector2<> point);
	};
}

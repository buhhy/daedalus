#pragma once

#include "BiomeRegionData.h"
#include "TerrainDataStructures.h"
#include <Utilities/DataStructures.h>
#include <Controllers/EventBus/EventBus.h>

#include <unordered_map>
#include <memory>

namespace terrain {
	struct BiomeTriangle {
	private:
		utils::Vector2D<> ApplyOffset(
			BiomeData const * biome,
			const BiomeRegionOffsetVector & offset
		) const {
			const auto & globalPos = biome->GetGlobalPosition();
			return (globalPos.first - offset).Cast<double>() + globalPos.second;
		}

	public:
		const std::array<BiomeData *, 3> Biomes;

		BiomeTriangle(BiomeData * const b1, BiomeData * const b2, BiomeData * const b3) :
			Biomes{{ b1, b2, b3 }}
		{}

		utils::UVWVector InterpolatePoint(const BiomePositionVector & position) const {
			utils::Triangle2D bounds {
				ApplyOffset(Biomes[0], position.first),
				ApplyOffset(Biomes[1], position.first),
				ApplyOffset(Biomes[2], position.first)
			};
			utils::UVWVector uvw;
			bounds.GetBarycentricCoordinates(uvw, position.second);
			return uvw;
		}

		BiomeData * operator [] (const Uint8 index) { return Biomes[index]; }
		const BiomeData * operator [] (const Uint8 index) const { return Biomes[index]; }
	};

	/**
	 * This class will load data related to a particular biome region. Biome regions will
	 * contain multiple biomes linked through a delaunay triangulation. After the generation
	 * of the triangulation and corresponding Voronoi diagram, we can generate specific
	 * features, allowing us to create more contextual terrain features like coherent mountain
	 * ranges and what not. This class will most likely run on the server-side.
	 */
	class BiomeRegionLoader {
	public:
		using VertexWithHullIndex = std::pair<utils::Vector2D<>, Uint32>;
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
		

		inline const BiomeGeneratorParameters & GetGeneratorParameters() const {
			return BiomeGenParams;
		}

		BiomeDataPtr GetBiomeAt(const BiomeId & id);
		BiomeRegionDataPtr GetBiomeRegionAt(const BiomeRegionOffsetVector & offset);
		const BiomeId FindNearestBiomeId(const utils::Vector2D<> point);
		const BiomeTriangle FindContainingBiomeTriangle(
			const utils::Vector2D<> point);
	};
}

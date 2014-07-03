#include "Daedalus.h"
#include "BiomeRegionData.h"

#include <assert.h>

namespace terrain {
	BiomeRegionData::BiomeRegionData(
		const uint16_t buffer,
		const uint32_t biomeSize,
		const BiomeRegionOffsetVector & biomeOffset
	) : BiomeGridSize(biomeSize),
		BiomeOffset(biomeOffset),
		BiomeCells(biomeSize, biomeSize),
		DelaunayGraph(biomeOffset),
		CurrentVertexId(0),
		NeighboursMerged(3, 3, false),
		bIsGraphGenerated(false),
		bIsBiomeDataGenerated(false)
	{
		// This region is technically always loaded, hence set to true
		NeighboursMerged.Set(1, 1, true);
	}

	uint64_t BiomeRegionData::AddBiome(
		const uint32_t x, const uint32_t y,
		const BiomeCellVertex & position
	) {
		auto id = GetNextId();
		Biomes.insert(std::make_pair(
			id, std::shared_ptr<BiomeData>(new BiomeData(BuildId(id), position))));
		BiomeCells.Get(x, y).AddPoint(id);
		return id;
	}

	std::tuple<uint64_t, BiomeRegionGridVector, double> BiomeRegionData::FindNearestPoint(
		utils::Vector2<> offset
	) const {
		double xpos = offset.X * BiomeGridSize;
		double ypos = offset.Y * BiomeGridSize;

		uint16_t xstart = std::max((uint32_t) std::floor(xpos) - 2, 0u);
		uint16_t xend = std::min((uint32_t) std::ceil(xpos) + 2, BiomeGridSize - 1);
		uint16_t ystart = std::max((uint32_t) std::floor(ypos) - 2, 0u);
		uint16_t yend = std::min((uint32_t) std::ceil(ypos) + 2, BiomeGridSize - 1);

		double min = 5.0;
		uint64_t vid = 0;
		BiomeRegionGridVector gpos;
		for (uint16_t x = xstart; x <= xend; x++) {
			for (uint16_t y = ystart; y <= yend; y++) {
				for (auto id : BiomeCells.Get(x, y).PointIds) {
					double dist = (offset - Biomes.at(id)->GetLocalPosition()).Length2();
					if (dist < min) {
						min = dist;
						vid = id;
						gpos.Reset(x, y);
					}
				}
			}
		}

		return std::make_tuple(vid, gpos, min);
	}

	void BiomeRegionData::GenerateDelaunayGraph() {
		std::vector<std::pair<BiomeCellVertex, uint64_t> > vertexList;
		for (auto & pair : Biomes)
			vertexList.push_back(std::make_pair(pair.second->GetLocalPosition(), pair.first));
		utils::BuildDelaunay2D(DelaunayGraph, vertexList);
		bIsGraphGenerated = true;
	}

	void BiomeRegionData::GenerateBiomeData() {
		// Do some biome data initialization
		bIsBiomeDataGenerated = true;
	}
}

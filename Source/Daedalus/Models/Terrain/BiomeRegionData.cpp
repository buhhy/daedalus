#include "Daedalus.h"
#include "BiomeRegionData.h"

#include <assert.h>

namespace terrain {
	BiomeRegionData::BiomeRegionData(
		const Uint16 buffer,
		const Uint32 biomeSize,
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

	bool BiomeRegionData::IsMergedWithAllNeighbours() const {
		bool done = true;
		for (Uint8 x = 0; x < NeighboursMerged.GetWidth(); x++) {
			for (Uint8 y = 0; y < NeighboursMerged.GetDepth(); y++) {
				if (!NeighboursMerged.Get(x, y)) {
					done = false;
					break;
				}
			}
		}
		return done;
	}

	Uint64 BiomeRegionData::AddBiome(
		const Uint32 x, const Uint32 y,
		const BiomeCellVertex & position
	) {
		auto id = GetNextId();
		Biomes.insert(std::make_pair(
			id, std::shared_ptr<BiomeData>(new BiomeData(BuildId(id), position))));
		BiomeCells.Get(x, y).AddPoint(id);
		return id;
	}

	std::tuple<Uint64, BiomeRegionGridVector, double> BiomeRegionData::FindNearestPoint(
		utils::Vector2<> offset
	) const {
		double xpos = offset.X * BiomeGridSize;
		double ypos = offset.Y * BiomeGridSize;

		Uint16 xstart = std::max((Uint32) std::floor(xpos) - 2, 0u);
		Uint16 xend = std::min((Uint32) std::ceil(xpos) + 2, BiomeGridSize - 1);
		Uint16 ystart = std::max((Uint32) std::floor(ypos) - 2, 0u);
		Uint16 yend = std::min((Uint32) std::ceil(ypos) + 2, BiomeGridSize - 1);

		double min = 5.0;
		Uint64 vid = 0;
		BiomeRegionGridVector gpos;
		for (Uint16 x = xstart; x <= xend; x++) {
			for (Uint16 y = ystart; y <= yend; y++) {
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

	void BiomeRegionData::GenerateDelaunayGraph(
		const utils::DelaunayBuilderDAC2D & builder
	) {
		std::vector<std::pair<BiomeCellVertex, Uint64> > vertexList;
		for (auto & pair : Biomes)
			vertexList.push_back(std::make_pair(pair.second->GetLocalPosition(), pair.first));
		builder.BuildDelaunayGraph(DelaunayGraph, vertexList);
		bIsGraphGenerated = true;
	}

	void BiomeRegionData::GenerateBiomeData() {
		// Do some biome data initialization
		bIsBiomeDataGenerated = true;
	}
}

#include "Daedalus.h"
#include "BiomeRegionData.h"

#include <assert.h>

namespace terrain {
	using utils::Vector2D;

	using NearestBiomeResult = BiomeRegionData::NearestBiomeResult;

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

	NearestBiomeResult BiomeRegionData::FindNearestPoint(Vector2D<> offset) const {
		double xpos = offset.X * BiomeGridSize;
		double ypos = offset.Y * BiomeGridSize;

		// If the current point is too close to the edge of the region, we must also search
		// the adjacent regions for closer biome points. (-1, -1) means to look at the 3
		// regions to the left and bottom of the current region. (0, 0) means no additional
		// search is required. (1, 1) means to look at the 3 regions to the top and right of
		// the current region.
		Vector2D<Int8> offmap(0, 0);

		auto xstart = std::floor(xpos) - 2;
		auto ystart = std::floor(ypos) - 2;
		auto xend = std::ceil(xpos) + 2;
		auto yend = std::ceil(ypos) + 2;

		if (xstart <= utils::FLOAT_ERROR) {
			xstart = 0;
			offmap.X = -1;
		}
		if (ystart <= utils::FLOAT_ERROR) {
			ystart = 0;
			offmap.Y = -1;
		}
		if (xend - BiomeGridSize > -utils::FLOAT_ERROR) {
			xend = BiomeGridSize - 1;
			offmap.X = 1;
		}
		if (yend - BiomeGridSize > -utils::FLOAT_ERROR) {
			yend = BiomeGridSize - 1;
			offmap.Y = 1;
		}

		double min = 5.0;
		Uint64 vid = 0;
		BiomeRegionGridVector gpos(0, 0);
		for (Uint32 x = xstart; x <= xend; x++) {
			for (Uint32 y = ystart; y <= yend; y++) {
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

		return NearestBiomeResult(vid, gpos, min, offmap);
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

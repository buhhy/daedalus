#include "Daedalus.h"
#include "BiomeRegionData.h"

#include <assert.h>

namespace terrain {
	BiomeRegionData::BiomeRegionData(
		const uint16 buffer,
		const uint32 biomeSize,
		const BiomeRegionOffsetVector & biomeOffset
	) : BiomeGridSize(biomeSize),
		BiomeOffset(biomeOffset),
		PointDistribution(biomeSize),
		DelaunayGraph(biomeOffset),
		CurrentVertexId(0),
		NeighborsLoaded(3, 3, false)
	{
		auto toY = PointDistribution.Depth - buffer - 1;
		auto toX = PointDistribution.Width - buffer - 1;
		// This region is technically always laoded, hence set to true
		NeighborsLoaded.Set(1, 1, true);
	}

	uint64 BiomeRegionData::AddBiome(
		const uint64 x, const uint64 y,
		const BiomeCellVertex & position
	) {
		auto id = GetNextId();
		Biomes.insert({ id, std::unique_ptr<BiomeData>(new BiomeData(BuildId(id), position)) });
		PointDistribution.Get(x, y).AddPoint(id);
		return id;
	}

	std::tuple<uint64, BiomeRegionGridVector, double> BiomeRegionData::FindNearestPoint(
		utils::Vector2<> offset
	) const {
		double xpos = offset.X * BiomeGridSize;
		double ypos = offset.Y * BiomeGridSize;

		uint16 xstart = std::max((uint32) std::floor(xpos) - 2, 0u);
		uint16 xend = std::min((uint32) std::ceil(xpos) + 2, BiomeGridSize - 1);
		uint16 ystart = std::max((uint32) std::floor(ypos) - 2, 0u);
		uint16 yend = std::min((uint32) std::ceil(ypos) + 2, BiomeGridSize - 1);

		double min = 5.0;
		uint64 vid = 0;
		BiomeRegionGridVector gpos;
		for (uint16 x = xstart; x <= xend; x++) {
			for (uint16 y = ystart; y <= yend; y++) {
				for (auto id : PointDistribution.Get(x, y).PointIds) {
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
}

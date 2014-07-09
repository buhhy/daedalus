#include "Daedalus.h"
#include "BiomeRegionData.h"

#include <assert.h>

namespace terrain {
	using namespace utils;

	using NearestBiomeResult = BiomeRegionData::NearestBiomeResult;
	using ContainingTriangleResult = BiomeRegionData::ContainingTriangleResult;

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
		Biomes.reserve(biomeSize * biomeSize);
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
		const Vector2D<> & position
	) {
		auto id = GetNextId();
		Biomes.insert(std::make_pair(
			id, std::unique_ptr<BiomeData>(
				new BiomeData(BuildId(id), BiomePositionVector(BiomeOffset, position)))));
		BiomeCells.Get(x, y).AddPoint(id);
		return id;
	}

	const NearestBiomeResult BiomeRegionData::FindNearestPoint(
		const Vector2D<> & position
	) const {
		auto coord = ToGridCoordinates(position);

		// If the current point is too close to the edge of the region, we must also search
		// the adjacent regions for closer biome points. (-1, -1) means to look at the 3
		// regions to the left and bottom of the current region. (0, 0) means no additional
		// search is required. (1, 1) means to look at the 3 regions to the top and right of
		// the current region.
		Vector2D<Int8> offmap(0, 0);

		auto xstart = coord.X - 2;
		auto ystart = coord.Y - 2;
		auto xend = coord.X + 3;
		auto yend = coord.Y + 3;

		if (xstart < 0) {
			xstart = 0;
			offmap.X = -1;
		}
		if (ystart < 0) {
			ystart = 0;
			offmap.Y = -1;
		}
		if (xend >= BiomeGridSize) {
			xend = BiomeGridSize - 1;
			offmap.X = 1;
		}
		if (yend >= BiomeGridSize) {
			yend = BiomeGridSize - 1;
			offmap.Y = 1;
		}

		double min = 5.0;
		Uint64 vid = 0;
		BiomeRegionGridVector gpos(0, 0);
		for (Uint32 x = xstart; x <= xend; x++) {
			for (Uint32 y = ystart; y <= yend; y++) {
				for (auto id : BiomeCells.Get(x, y).PointIds) {
					double dist = (position - Biomes.at(id)->GetLocalPosition()).Length2();
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

	const ContainingTriangleResult BiomeRegionData::FindContainingTriangle(
		const Vector2D<> & position
	) const {
		auto coord = ToGridCoordinates(position);

		// If the current point is too close to the edge of the region, we must also search
		// the adjacent regions for closer biome points. (-1, -1) means to look at the 3
		// regions to the left and bottom of the current region. (0, 0) means no additional
		// search is required. (1, 1) means to look at the 3 regions to the top and right of
		// the current region.
		Vector2D<Int8> offmap(0, 0);

		auto xstart = coord.X - 2;
		auto ystart = coord.Y - 2;
		auto xend = coord.X + 3;
		auto yend = coord.Y + 3;

		if (xstart < 0) {
			xstart = 0;
			offmap.X = -1;
		}
		if (ystart < 0) {
			ystart = 0;
			offmap.Y = -1;
		}
		if (xend >= BiomeGridSize) {
			xend = BiomeGridSize - 1;
			offmap.X = 1;
		}
		if (yend >= BiomeGridSize) {
			yend = BiomeGridSize - 1;
			offmap.Y = 1;
		}

		for (Uint32 x = xstart; x <= xend; x++) {
			for (Uint32 y = ystart; y <= yend; y++) {
				for (auto id : BiomeCells.Get(x, y).PointIds) {
					auto face = DelaunayGraph.FindVertex(id)->FindFaceContainingPoint(position);
					if (face != NULL) {
						auto vertices = face->GetVertices();
						BiomeTriangleIds results;
						std::transform(vertices.cbegin(), vertices.cend(), results.begin(),
							[&] (const delaunay::Vertex * v) {
								return BiomeId(v->ParentGraphOffset(), v->ForeignVertexId());
							});
						return ContainingTriangleResult(
							Option<BiomeTriangleIds>(results), Vector2D<Int8>(0, 0));
					}
				}
			}
		}

		return ContainingTriangleResult(Option<BiomeTriangleIds>(), offmap);
	}

	void BiomeRegionData::GenerateDelaunayGraph(
		const DelaunayBuilderDAC2D & builder
	) {
		std::vector<std::pair<Vector2D<>, Uint64> > vertexList;
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

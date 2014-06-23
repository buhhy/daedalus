#include "Daedalus.h"
#include "BiomeRegionLoader.h"
#include "Hash.h"

#include <algorithm>
#include <functional>
#include <random>

namespace terrain {
	typedef std::pair<utils::Vector2<>, uint64> VertexWithHullIndex;

	BiomeRegionLoader::BiomeRegionLoader(
		const BiomeGeneratorParameters & params,
		TSharedRef<events::EventBus> eventBus
	) : BiomeGenParams(params), EventBus(eventBus), CornerLimit(2)
	{}

	BiomeRegionLoader::~BiomeRegionLoader() {
		LoadedBiomeRegionCache.empty();
	}

	TSharedPtr<const VertexWithHullIndex> BiomeRegionLoader::GetCornerHullVertex(
		const BiomeRegionData & data,
		const bool cornerX,
		const bool cornerY
	) const {
		const int8 dirX = cornerX ? -1 : 1;
		const int8 dirY = cornerY ? -1 : 1;
		const uint64 startX = cornerX ? data.BiomeGridSize.X - 1 : 0;
		const uint64 startY = cornerY ? data.BiomeGridSize.Y - 1 : 0;
		int8 accumX = 0, accumY = 0;
		const VertexWithHullIndex * vertex = NULL;
		bool found;

		do {
			found = false;
			const auto & pointIds =
				data.PointDistribution.Get(startX + accumX, startY + accumY).PointIds;
			for (uint64 i = 0; i < pointIds.size(); i++) {
				auto value = data.DelaunayGraph.ConvexHull.FindVertexById(pointIds[i]);
				if (value != -1) {
					found = true;
					vertex = new VertexWithHullIndex(data.Points.at(pointIds[i]), value);
				}
			}
			if (accumX * accumX > accumY * accumY)
				accumY += dirY;
			else
				accumX += dirX;
		} while (!found && (accumX < CornerLimit && accumY < CornerLimit));

		if (found)
			return MakeShareable(vertex);
		else
			return NULL;
	};

	bool BiomeRegionLoader::MergeRegionEdge(BiomeRegionData & r1, BiomeRegionData & r2) {
		auto direction = r2.BiomeOffset - r1.BiomeOffset;
		auto * region1 = &r1, * region2 = &r2;
		TSharedPtr<const VertexWithHullIndex> upperLimit1 = NULL;
		TSharedPtr<const VertexWithHullIndex> upperLimit2 = NULL;
		TSharedPtr<const VertexWithHullIndex> lowerLimit1 = NULL;
		TSharedPtr<const VertexWithHullIndex> lowerLimit2 = NULL;

		if (direction.X == -1 && direction.Y == 0 || direction.X == 0 && direction.Y == -1) {
			std::swap(region1, region2);
			direction.X = direction.X * -1;
			direction.Y = direction.Y * -1;
		}

		if (direction.X == 1 && direction.Y == 0) {
			// Merge right
			lowerLimit1 = GetCornerHullVertex(*region1, true, false);
			lowerLimit2 = GetCornerHullVertex(*region2, false, false);
			upperLimit1 = GetCornerHullVertex(*region1, true, true);
			upperLimit2 = GetCornerHullVertex(*region2, false, true);
		} else if (direction.X == 0 && direction.Y == 1) {
			// Merge up
			lowerLimit1 = GetCornerHullVertex(*region1, true, true);
			lowerLimit2 = GetCornerHullVertex(*region2, true, false);
			upperLimit1 = GetCornerHullVertex(*region1, false, true);
			upperLimit2 = GetCornerHullVertex(*region2, false, false);
		}

		// Logging output
		UE_LOG(LogTemp, Warning, TEXT("Merging edges: (%ld %ld) - (%ld %ld)"),
			region1->BiomeOffset.X, region1->BiomeOffset.Y,
			region2->BiomeOffset.X, region2->BiomeOffset.Y)

		if (upperLimit1.IsValid() && upperLimit2.IsValid())
			UE_LOG(LogTemp, Warning, TEXT("Upper limit: %ld:(%f %f) - %ld:(%f %f)"),
				upperLimit1->second, upperLimit1->first.X, upperLimit1->first.Y,
				upperLimit2->second, upperLimit2->first.X, upperLimit2->first.Y)
		if (lowerLimit1.IsValid() && lowerLimit2.IsValid())
			UE_LOG(LogTemp, Warning, TEXT("Lower limit: %ld:(%f %f) - %ld:(%f %f)"),
				lowerLimit1->second, lowerLimit1->first.X, lowerLimit1->first.Y,
				lowerLimit2->second, lowerLimit2->first.X, lowerLimit2->first.Y)
		
		// Merge the 2 regions
		if (upperLimit1.IsValid() && upperLimit2.IsValid() &&
				lowerLimit1.IsValid() && lowerLimit2.IsValid()) {
			utils::MergeDelaunayTileEdge(
				region1->DelaunayGraph, region2->DelaunayGraph,
				lowerLimit1->second, lowerLimit2->second,
				upperLimit1->second, upperLimit2->second);
			return true;
		}
		return false;
	}

	bool BiomeRegionLoader::MergeRegionCorner(
		BiomeRegionData & tl,
		BiomeRegionData & tr,
		BiomeRegionData & bl,
		BiomeRegionData & br
	) {
		const uint8 size = 4;
		std::array<BiomeRegionData *, size> data = {{ &tl, &tr, &bl, &br }};
		std::array<TSharedPtr<const VertexWithHullIndex>, size> vertices = {{
			GetCornerHullVertex(tl, true, false),
			GetCornerHullVertex(tr, false, false),
			GetCornerHullVertex(bl, true, true),
			GetCornerHullVertex(br, false, true)
		}};

		std::array<std::pair<utils::DelaunayGraph *, uint64>, size> input;

		for (uint8 i = 0; i < size; i++)
			input[i] = std::make_pair(&data[i]->DelaunayGraph, vertices[i]->second);

		utils::MergeDelaunayTileCorner(input);

		// TODO: implement edge flipping
		UE_LOG(LogTemp, Warning, TEXT("Merging corners: (%ld %ld) - (%ld %ld) - (%ld %ld) - (%ld %ld)"),
			tl.BiomeOffset.X, tl.BiomeOffset.Y, tr.BiomeOffset.X, tr.BiomeOffset.Y,
			bl.BiomeOffset.X, bl.BiomeOffset.Y, br.BiomeOffset.X, br.BiomeOffset.Y)

		return false;
	}
	
	std::vector<BiomeOffsetVector> BiomeRegionLoader::MergeRegion(
		TSharedRef<BiomeRegionData> targetRegion,
		const BiomeOffsetVector & biomeOffset
	) {
		// Update neighbor's neighbor generation cache
		utils::Tensor2<TSharedPtr<BiomeRegionData> > neighbors(3, 3, NULL);
		BiomeOffsetVector nOffset;

		// Load up the neighboring biome regions if they haven't already been cached
		for (int8 offY = -1; offY <= 1; offY++) {
			for (int8 offX = -1; offX <= 1; offX++) {
				if (offY != 0 || offX != 0) {
					nOffset.Reset(offX + biomeOffset.X, offY + biomeOffset.Y);
					if (LoadedBiomeRegionCache.count(nOffset) > 0)
						neighbors.Set(offX + 1, offY + 1, LoadedBiomeRegionCache.at(nOffset));
					else
						neighbors.Set(offX + 1, offY + 1, LoadBiomeRegionFromDisk(nOffset));
				}
			}
		}
		neighbors.Set(1, 1, targetRegion);

		std::vector<BiomeOffsetVector> mergedRegions;

		// Merge shared edges
		for (int8 offY = -1; offY <= 1; offY++) {
			for (int8 offX = -1; offX <= 1; offX++) {
				uint8 dist = offY * offY + offX * offX;
				if (dist == 1) { // edge
					auto region = neighbors.Get(offX + 1, offY + 1);
					if (region.IsValid()) {
						if (MergeRegionEdge(*targetRegion, *region))
							mergedRegions.push_back(region->BiomeOffset);
					}
				}
			}
		}

		// Merge shared corners
		for (int8 offY = -1; offY <= 0; offY++) {
			for (int8 offX = -1; offX <= 0; offX++) {
				auto blr = neighbors.Get(offX + 1, offY + 1);
				auto brr = neighbors.Get(offX + 2, offY + 1);
				auto tlr = neighbors.Get(offX + 1, offY + 2);
				auto trr = neighbors.Get(offX + 2, offY + 2);
				if (blr.IsValid() && brr.IsValid() && tlr.IsValid() && trr.IsValid())
					MergeRegionCorner(*tlr, *trr, *blr, *brr);
			}
		}

		return mergedRegions;
	}

	TSharedPtr<BiomeRegionData> BiomeRegionLoader::LoadBiomeRegionFromDisk(
		const BiomeOffsetVector & offset
	) const {
		// TODO: implement disk loading
		return TSharedPtr<BiomeRegionData>(NULL);
	}

	TSharedRef<BiomeRegionData> BiomeRegionLoader::GenerateMissingBiomeRegion(
		const BiomeOffsetVector & biomeOffset
	) const {
		// TODO: implement disk saving
		auto data = new BiomeRegionData(
			BiomeGenParams.BufferSize, BiomeGenParams.BiomeGridCellSize, biomeOffset);
		auto dataRef = TSharedRef<BiomeRegionData>(data);

		// Initialize Mersenne Twister PRNG with seed, this guarantees each region will
		// always have the same sequence of random numbers
		auto mtSeed = utils::HashFromVector(BiomeGenParams.Seed, biomeOffset);
		auto randNumPoints = std::bind(
			std::uniform_int_distribution<int>(
				BiomeGenParams.MinPointsPerCell,
				BiomeGenParams.MaxPointsPerCell), std::mt19937(mtSeed));
		auto randPosition = std::bind(
			std::uniform_real_distribution<double>(0.1, 0.9), std::mt19937(mtSeed));

		uint16 numPoints = 0;
		utils::Vector2<> point;
		utils::Vector2<> offset;
		utils::Vector2<> biomeRegionSize = BiomeGenParams.BiomeGridCellSize.Cast<double>();

		// Run Delaunay triangulation algorithm
		std::vector<std::pair<BiomeCellVertex, uint64> > vertexList;
		
		// Create uniform random point distribution, and insert vertices into aggregate list
		for (auto y = 0; y < BiomeGenParams.BiomeGridCellSize.Y; y++) {
			for (auto x = 0; x < BiomeGenParams.BiomeGridCellSize.X; x++) {
				numPoints = randNumPoints();
				offset.Reset(x, y);
				for (auto n = numPoints - 1; n >= 0; n--) {
					// Set point X, Y to random point within cell
					point.Reset(randPosition(), randPosition());
					point = (point + offset) / biomeRegionSize;
					auto id = data->InsertPoint(x, y, point);

					// Align X, Y relative to entire region
					vertexList.push_back(std::make_pair(point, id));
				}
			}
		}

		utils::BuildDelaunay2D(data->DelaunayGraph, vertexList);

		return dataRef;
	}

	TSharedRef<BiomeRegionData> BiomeRegionLoader::GetBiomeRegionAt(
		const BiomeOffsetVector & offset
	) {
		//UE_LOG(LogTemp, Error, TEXT("Loading chunk at offset: %d %d %d"), offset.X, offset.Y, offset.Z);
		if (LoadedBiomeRegionCache.count(offset) > 0) {
			return LoadedBiomeRegionCache.at(offset);
		} else {
			auto loaded = LoadBiomeRegionFromDisk(offset);
			if (loaded.IsValid()) {
				LoadedBiomeRegionCache.insert({ offset, loaded.ToSharedRef() });
				return loaded.ToSharedRef();
			} else {
				auto generated = GenerateMissingBiomeRegion(offset);
				auto updatedRegions = MergeRegion(generated, offset);

				// We will need to fire off an update event since adjacent regions may have
				// been merged.
				if (!updatedRegions.empty()) {
					EventBus->BroadcastEvent(
						events::E_BiomeRegionUpdate,
						MakeShareable(new events::EBiomeRegionUpdate(updatedRegions)));
				}

				LoadedBiomeRegionCache.insert({ offset, generated });
				return generated;
			}
		}
	}

	const BiomeGeneratorParameters & BiomeRegionLoader::GetGeneratorParameters() const {
		return BiomeGenParams;
	}
}

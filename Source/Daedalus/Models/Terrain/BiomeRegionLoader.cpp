#include <Daedalus.h>
#include "BiomeRegionLoader.h"

#include <Utilities/Hash.h>
#include <Utilities/Noise/Perlin.h>

#include <algorithm>
#include <functional>
#include <random>

namespace terrain {
	using VertexWithHullIndex = BiomeRegionLoader::VertexWithHullIndex;
	using BiomeRegionDataPtr = BiomeRegionLoader::BiomeRegionDataPtr;
	using DelaunayBuilderPtr = BiomeRegionLoader::DelaunayBuilderPtr;

	BiomeRegionLoader::BiomeRegionLoader(
		const BiomeGeneratorParameters & params,
		std::shared_ptr<events::EventBus> eventBus,
		DelaunayBuilderPtr builder
	) : BiomeGenParams(params), EventBus(eventBus), DelaunayBuilder(builder)
	{}

	BiomeRegionLoader::~BiomeRegionLoader() {
		LoadedBiomeRegionCache.empty();
	}

	std::shared_ptr<const VertexWithHullIndex> BiomeRegionLoader::GetCornerHullVertex(
		const BiomeRegionData & data,
		const bool cornerX, const bool cornerY
	) const {
		const int8_t dirX = cornerX ? -1 : 1;
		const int8_t dirY = cornerY ? -1 : 1;
		const uint32_t startX = cornerX ? data.GetBiomeGridSize() - 1 : 0;
		const uint32_t startY = cornerY ? data.GetBiomeGridSize() - 1 : 0;
		const uint16_t cornerLimit = BiomeGenParams.BufferSize;
		const VertexWithHullIndex * vertex = NULL;

		int8_t accumX = 0, accumY = 0;
		bool found;

		do {
			found = false;
			const auto & pointIds =
				data.GetBiomeCells().Get(startX + accumX, startY + accumY).PointIds;
			for (size_t i = 0; i < pointIds.size(); i++) {
				auto value = data.DelaunayGraph.ConvexHull.FindVertexById(pointIds[i]);
				if (value != -1) {
					found = true;
					vertex = new VertexWithHullIndex(
						data.GetBiomeAt(pointIds[i])->GetLocalPosition(), value);
				}
			}
			if (accumX * accumX > accumY * accumY)
				accumY += dirY;
			else
				accumX += dirX;
		} while (!found && (accumX < cornerLimit && accumY < cornerLimit));

		if (found)
			return std::shared_ptr<const VertexWithHullIndex>(vertex);
		else
			return NULL;
	};

	bool BiomeRegionLoader::MergeRegionEdge(BiomeRegionData & r1, BiomeRegionData & r2) {
		auto direction = r2.GetBiomeRegionOffset() - r1.GetBiomeRegionOffset();
		auto * region1 = &r1, * region2 = &r2;
		std::shared_ptr<const VertexWithHullIndex> upperLimit1 = NULL;
		std::shared_ptr<const VertexWithHullIndex> upperLimit2 = NULL;
		std::shared_ptr<const VertexWithHullIndex> lowerLimit1 = NULL;
		std::shared_ptr<const VertexWithHullIndex> lowerLimit2 = NULL;

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
			region1->GetBiomeRegionOffset().X, region1->GetBiomeRegionOffset().Y,
			region2->GetBiomeRegionOffset().X, region2->GetBiomeRegionOffset().Y)
		
		// Merge the 2 regions
		if (upperLimit1 && upperLimit2 && lowerLimit1 && lowerLimit2) {
			DelaunayBuilder->MergeDelaunayTileEdge(
				region1->DelaunayGraph, region2->DelaunayGraph,
				lowerLimit1->second, lowerLimit2->second,
				upperLimit1->second, upperLimit2->second);
			
			// Set the neighbor flags
			region1->NeighboursMerged.Set(direction.X + 1, direction.Y + 1, true);
			region2->NeighboursMerged.Set(-direction.X + 1, -direction.Y + 1, true);

			return true;
		}

		return false;
	}

	bool BiomeRegionLoader::MergeRegionCorner(
		BiomeRegionData & tl, BiomeRegionData & tr,
		BiomeRegionData & bl, BiomeRegionData & br
	) {
		const uint8_t size = 4;
		std::array<BiomeRegionData *, size> data = {{ &tl, &tr, &bl, &br }};
		std::array<std::shared_ptr<const VertexWithHullIndex>, size> vertices = {{
			GetCornerHullVertex(tl, true, false),
			GetCornerHullVertex(tr, false, false),
			GetCornerHullVertex(bl, true, true),
			GetCornerHullVertex(br, false, true)
		}};

		std::array<std::pair<utils::DelaunayGraph *, uint32_t>, size> input;

		for (uint8_t i = 0; i < size; i++)
			input[i] = std::make_pair(&data[i]->DelaunayGraph, vertices[i]->second);

		DelaunayBuilder->MergeDelaunayTileCorner(input);
		
		// Set the neighbor flags
		tl.NeighboursMerged.Set(2, 0, true);
		tr.NeighboursMerged.Set(0, 0, true);
		bl.NeighboursMerged.Set(2, 2, true);
		br.NeighboursMerged.Set(0, 2, true);

		UE_LOG(LogTemp, Warning, TEXT("Merging corners: (%ld %ld) - (%ld %ld) - (%ld %ld) - (%ld %ld)"),
			tl.GetBiomeRegionOffset().X, tl.GetBiomeRegionOffset().Y,
			tr.GetBiomeRegionOffset().X, tr.GetBiomeRegionOffset().Y,
			bl.GetBiomeRegionOffset().X, bl.GetBiomeRegionOffset().Y,
			br.GetBiomeRegionOffset().X, br.GetBiomeRegionOffset().Y)

		return false;
	}
	
	std::unordered_set<BiomeRegionOffsetVector> BiomeRegionLoader::MergeRegion(
		BiomeRegionDataPtr targetRegion
	) {
		// If this region has already been merged with all surrounding regions, then skip
		if (targetRegion->IsMergedWithAllNeighbours())
			return std::unordered_set<BiomeRegionOffsetVector>();

		const BiomeRegionOffsetVector & biomeOffset = targetRegion->GetBiomeRegionOffset();

		// Update neighbor's neighbor generation cache
		utils::TensorFixed2D<BiomeRegionDataPtr, 3> neighbors(NULL);
		BiomeRegionOffsetVector currentOffset;

		// Load up the neighboring biome regions if they haven't already been cached
		// and the sides haven't already been merged
		for (int8_t offY = -1; offY <= 1; offY++) {
			for (int8_t offX = -1; offX <= 1; offX++) {
				if (offY != 0 || offX != 0) {
					currentOffset.Reset(offX + biomeOffset.X, offY + biomeOffset.Y);
					neighbors.Set(offX + 1, offY + 1, GetBiomeRegionFromCache(currentOffset));
				}
			}
		}
		neighbors.Set(1, 1, targetRegion);

		std::unordered_set<BiomeRegionOffsetVector> mergedRegions;

		// Merge shared edges
		for (int8_t offY = -1; offY <= 1; offY++) {
			for (int8_t offX = -1; offX <= 1; offX++) {
				if ((offY == 0) ^ (offX == 0)) { // edge
					// If the side hasn't been merged
					if (!targetRegion->NeighboursMerged.Get(offX + 1, offY + 1)) {
						auto region = neighbors.Get(offX + 1, offY + 1);
						if (region && MergeRegionEdge(*targetRegion, *region))
							mergedRegions.insert(region->GetBiomeRegionOffset());
					}
				}
			}
		}

		// Merge shared corners
		for (int8_t offY = -1; offY <= 0; offY++) {
			for (int8_t offX = -1; offX <= 0; offX++) {
				auto blr = neighbors.Get(offX + 1, offY + 1);
				auto brr = neighbors.Get(offX + 2, offY + 1);
				auto tlr = neighbors.Get(offX + 1, offY + 2);
				auto trr = neighbors.Get(offX + 2, offY + 2);

				// If all the corners exist, and the corner hasn't been merged yet
				if (blr && brr && tlr && trr &&
						!blr->NeighboursMerged.Get(2, 2) &&
						MergeRegionCorner(*tlr, *trr, *blr, *brr)) {
					mergedRegions.insert(blr->GetBiomeRegionOffset());
					mergedRegions.insert(blr->GetBiomeRegionOffset());
					mergedRegions.insert(tlr->GetBiomeRegionOffset());
					mergedRegions.insert(trr->GetBiomeRegionOffset());
				}	
			}
		}

		return mergedRegions;
	}

	bool BiomeRegionLoader::IsBiomeRegionGenerated(
		const BiomeRegionOffsetVector & offset
	) const {
		// TODO: implement some form of tracking which biome regions have been generated
		return LoadedBiomeRegionCache.find(offset) != LoadedBiomeRegionCache.end();
	}
	
	BiomeRegionDataPtr BiomeRegionLoader::GetBiomeRegionFromCache(
		const BiomeRegionOffsetVector & offset
	) {
		auto found = LoadedBiomeRegionCache.find(offset);
		if (found != LoadedBiomeRegionCache.end())
			return found->second;
		else
			return LoadBiomeRegionFromDisk(offset);
	}

	BiomeRegionDataPtr BiomeRegionLoader::LoadBiomeRegionFromDisk(
		const BiomeRegionOffsetVector & offset
	) {
		// TODO: implement disk loading
		if (IsBiomeRegionGenerated(offset))
			return NULL;
		return NULL;
	}

	BiomeRegionDataPtr BiomeRegionLoader::GenerateBiomeRegion(
		const BiomeRegionOffsetVector & biomeOffset
	) {
		// TODO: implement disk saving
		auto dataRef = BiomeRegionDataPtr(
			new BiomeRegionData(
				BiomeGenParams.BufferSize, BiomeGenParams.GridCellCount, biomeOffset));

		// Initialize Mersenne Twister PRNG with seed, this guarantees each region will
		// always have the same sequence of random numbers
		auto mtSeed = utils::HashFromVector(BiomeGenParams.Seed, biomeOffset);
		auto randNumPoints = std::bind(
			std::uniform_int_distribution<int>(
				BiomeGenParams.MinPointsPerCell,
				BiomeGenParams.MaxPointsPerCell), std::mt19937(mtSeed));
		auto randPosition = std::bind(
			std::uniform_real_distribution<double>(0.1, 0.9), std::mt19937(mtSeed));

		uint16_t numPoints = 0;
		BiomeCellVertex point;
		utils::Vector2<> offset;
		
		// Create uniform random point distribution, and insert vertices into aggregate list
		for (auto y = 0u; y < BiomeGenParams.GridCellCount; y++) {
			for (auto x = 0u; x < BiomeGenParams.GridCellCount; x++) {
				numPoints = randNumPoints();
				offset.Reset(x, y);
				for (auto n = numPoints - 1; n >= 0; n--) {
					// Set point X, Y to random point within cell
					point.Reset(randPosition(), randPosition());
					point = (point + offset) / (double) BiomeGenParams.GridCellCount;
					auto id = dataRef->AddBiome(x, y, point);
				}
			}
		}

		// Run Delaunay triangulation algorithm
		dataRef->GenerateDelaunayGraph(*DelaunayBuilder);

		LoadedBiomeRegionCache.insert({ biomeOffset, dataRef });

		return dataRef;
	}

	BiomeRegionDataPtr BiomeRegionLoader::GenerateBiomeRegionArea(
		const BiomeRegionOffsetVector & offset,
		const uint8_t radius
	) {
		const uint8_t diameter = radius * 2 + 1;

		// Generate 8 surrounding regions as well as the current region
		BiomeRegionDataPtr newRegion;
		BiomeRegionOffsetVector currentOffset;
		utils::Tensor2D<BiomeRegionDataPtr> loadedRegions(diameter, diameter);

		for (int64_t offY = 0; offY < diameter; offY++) {
			for (int64_t offX = 0; offX < diameter; offX++) {
				currentOffset.Reset(offset.X - radius + offX, offset.Y - radius + offY);
				// Don't generate region if it has already been generated
				auto currentRegion = GetBiomeRegionFromCache(currentOffset);
				if (!currentRegion)
					currentRegion = GenerateBiomeRegion(currentOffset);
				loadedRegions.Set(offX, offY, currentRegion);
			}
		}
		newRegion = loadedRegions.Get(radius, radius);

		// Run the merge algorithm on all generated regions
		std::unordered_set<BiomeRegionOffsetVector> updatedRegions;
		for (int64_t offY = 0; offY < diameter; offY++) {
			for (int64_t offX = 0; offX < diameter; offX++) {
				auto currentRegion = loadedRegions.Get(offX, offY);
				auto updated = MergeRegion(currentRegion);
				updatedRegions.insert(currentRegion->GetBiomeRegionOffset());
				for (auto & up : updated)
					updatedRegions.insert(up);
			}
		}

		// We will need to fire off an update event since adjacent regions may have
		// been merged.
		if (!updatedRegions.empty()) {
			std::vector<BiomeRegionOffsetVector> updatedVec;
			updatedVec.insert(
				updatedVec.begin(), updatedRegions.cbegin(), updatedRegions.cend());
			EventBus->BroadcastEvent(
				events::E_BiomeRegionUpdate,
				std::shared_ptr<events::EBiomeRegionUpdate>(
					new events::EBiomeRegionUpdate(updatedVec)));
		}
		return newRegion;
	}

	BiomeRegionDataPtr BiomeRegionLoader::GenerateBiomeDataForRegion(
		BiomeRegionDataPtr biomeRegion
	) {
		utils::PerlinNoise2D generator(1234);
		// TODO: implement disk storage
		if (!biomeRegion->IsBiomeDataGenerated()) {
			auto & biomeCells = biomeRegion->GetBiomeCells();
			for (size_t x = 0; x < biomeCells.GetWidth(); x++) {
				for (size_t y = 0; y < biomeCells.GetDepth(); y++) {
					for (auto & id : biomeCells.Get(x, y).PointIds) {
						auto biome = biomeRegion->GetBiomeAt(id);
						auto position = biome->GetLocalPosition();
						auto height = generator.GenerateFractal(
							(position.X * biomeRegion->GetBiomeRegionOffset().X) * 0.017,
							(position.Y * biomeRegion->GetBiomeRegionOffset().Y) * 0.017,
							6, 0.5);
						biome->SetElevation(height);
					}
				}
			}

			std::vector<BiomeRegionOffsetVector> updatedVec;
			updatedVec.push_back(biomeRegion->GetBiomeRegionOffset());
			EventBus->BroadcastEvent(
				events::E_BiomeRegionUpdate,
				std::shared_ptr<events::EBiomeRegionUpdate>(
					new events::EBiomeRegionUpdate(updatedVec)));
		}
		return biomeRegion;
	} 

	BiomeRegionDataPtr BiomeRegionLoader::GetBiomeRegionAt(
		const BiomeRegionOffsetVector & offset
	) {
		//UE_LOG(LogTemp, Error, TEXT("Loading chunk at offset: %d %d %d"), offset.X, offset.Y, offset.Z);
		if (LoadedBiomeRegionCache.count(offset) > 0) {
			return LoadedBiomeRegionCache.at(offset);
		} else {
			if (IsBiomeRegionGenerated(offset))
				return LoadBiomeRegionFromDisk(offset);
			else
				return GenerateBiomeRegionArea(offset, 0);
		}
	}

	const BiomeId BiomeRegionLoader::FindNearestBiomeId(const utils::Vector2<> point) {
		// Convert into region coordinates
		const auto offset = BiomeGenParams.ToBiomeRegionCoordinates(point);
		const auto position = BiomeGenParams.GetInnerRegionPosition(point, offset);
		const auto foundResults = GetBiomeRegionAt(offset)->FindNearestPoint(position);

		const auto & foundGridOffset = std::get<1>(foundResults);
		const int8_t startX = foundGridOffset.X < 2 ? -1 : 0;
		const int8_t startY = foundGridOffset.Y < 2 ? -1 : 0;
		const int8_t endX = foundGridOffset.X >= BiomeGenParams.GridCellCount - 2 ? 1 : 0;
		const int8_t endY = foundGridOffset.Y >= BiomeGenParams.GridCellCount - 2 ? 1 : 0;

		BiomeRegionOffsetVector foundGlobalOffset(offset);
		uint64_t vid = std::get<0>(foundResults);
		double min = std::get<2>(foundResults);

		// If the found vertex is on the very edge of that particular region, then we must
		// search surrounding regions to ensure we have the nearest possible vertex to
		// the given location.
		for (int8_t x = startX; x <= endX; x++) {
			for (int8_t y = startY; y <= endY; y++) {
				if (x != 0 || y != 0) {
					auto newOffsetVector = BiomeRegionOffsetVector(offset.X + x, offset.Y + y);
					auto compare = GetBiomeRegionAt(newOffsetVector)
						->FindNearestPoint({ position.X + x, position.Y + y });
					auto dist = std::get<2>(compare);
					if (min > dist) {
						min = dist;
						vid = std::get<0>(compare);
						foundGlobalOffset.Reset(newOffsetVector);
					}
				}
			}
		}

		return BiomeId(foundGlobalOffset, vid);
	}
}

#include <Daedalus.h>
#include "BiomeRegionLoader.h"

#include <Utilities/Hash.h>

#include <algorithm>
#include <functional>
#include <random>

namespace terrain {
	typedef BiomeRegionLoader::VertexWithHullIndex VertexWithHullIndex;

	BiomeRegionLoader::BiomeRegionLoader(
		const BiomeGeneratorParameters & params,
		std::shared_ptr<events::EventBus> eventBus
	) : BiomeGenParams(params), EventBus(eventBus)
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
		const uint32_t startX = cornerX ? data.BiomeGridSize - 1 : 0;
		const uint32_t startY = cornerY ? data.BiomeGridSize - 1 : 0;
		const uint16_t cornerLimit = BiomeGenParams.BufferSize;
		int8_t accumX = 0, accumY = 0;
		const VertexWithHullIndex * vertex = NULL;
		bool found;

		do {
			found = false;
			const auto & pointIds =
				data.PointDistribution.Get(startX + accumX, startY + accumY).PointIds;
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
		auto direction = r2.BiomeOffset - r1.BiomeOffset;
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
			region1->BiomeOffset.X, region1->BiomeOffset.Y,
			region2->BiomeOffset.X, region2->BiomeOffset.Y)
		
		// Merge the 2 regions
		if (upperLimit1 && upperLimit2 && lowerLimit1 && lowerLimit2) {
			utils::MergeDelaunayTileEdge(
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

		utils::MergeDelaunayTileCorner(input);
		
		// Set the neighbor flags
		tl.NeighboursMerged.Set(2, 0, true);
		tr.NeighboursMerged.Set(0, 0, true);
		bl.NeighboursMerged.Set(2, 2, true);
		br.NeighboursMerged.Set(0, 2, true);

		UE_LOG(LogTemp, Warning, TEXT("Merging corners: (%ld %ld) - (%ld %ld) - (%ld %ld) - (%ld %ld)"),
			tl.BiomeOffset.X, tl.BiomeOffset.Y, tr.BiomeOffset.X, tr.BiomeOffset.Y,
			bl.BiomeOffset.X, bl.BiomeOffset.Y, br.BiomeOffset.X, br.BiomeOffset.Y)

		return false;
	}
	
	std::unordered_set<BiomeRegionOffsetVector> BiomeRegionLoader::MergeRegion(
		std::shared_ptr<BiomeRegionData> targetRegion
	) {
		const BiomeRegionOffsetVector & biomeOffset = targetRegion->BiomeOffset;

		// Update neighbor's neighbor generation cache
		utils::Tensor2<std::shared_ptr<BiomeRegionData> > neighbors(3, 3, NULL);
		BiomeRegionOffsetVector nOffset;

		// Load up the neighboring biome regions if they haven't already been cached
		// and the sides haven't already been merged
		for (int8_t offY = -1; offY <= 1; offY++) {
			for (int8_t offX = -1; offX <= 1; offX++) {
				if (offY != 0 || offX != 0) {
					nOffset.Reset(offX + biomeOffset.X, offY + biomeOffset.Y);
					if (IsBiomeRegionGenerated(nOffset))
						neighbors.Set(offX + 1, offY + 1, GetBiomeRegionAt(nOffset));
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
							mergedRegions.insert(region->BiomeOffset);
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
					mergedRegions.insert(blr->BiomeOffset);
					mergedRegions.insert(blr->BiomeOffset);
					mergedRegions.insert(tlr->BiomeOffset);
					mergedRegions.insert(trr->BiomeOffset);
				}	
			}
		}

		mergedRegions.erase(BiomeRegionOffsetVector(0, 0));

		return mergedRegions;
	}

	bool BiomeRegionLoader::IsBiomeRegionGenerated(
		const BiomeRegionOffsetVector & offset
	) const {
		// TODO: implement some form of tracking which biome regions have been generated
		return LoadedBiomeRegionCache.count(offset) > 0;
	}

	std::shared_ptr<BiomeRegionData> BiomeRegionLoader::LoadBiomeRegionFromDisk(
		const BiomeRegionOffsetVector & offset
	) {
		// TODO: implement disk loading
		return std::shared_ptr<BiomeRegionData>(NULL);
	}

	std::shared_ptr<BiomeRegionData> BiomeRegionLoader::GenerateBiomeRegion(
		const BiomeRegionOffsetVector & biomeOffset
	) {
		// TODO: implement disk saving
		auto dataRef = std::shared_ptr<BiomeRegionData>(
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
		dataRef->GenerateDelaunayGraph();

		LoadedBiomeRegionCache.insert({ biomeOffset, dataRef });

		return dataRef;
	}

	std::shared_ptr<BiomeRegionData> BiomeRegionLoader::GenerateBiomeRegionArea(
		const BiomeRegionOffsetVector & offset,
		const uint8_t radius
	) {
		std::shared_ptr<BiomeRegionData> newRegion;
		for (int64_t y = offset.Y - radius; y <= offset.Y + radius; y++) {
			for (int64_t x = offset.X - radius; x <= offset.X + radius; x++) {
				auto position = BiomeRegionOffsetVector(x, y);
				auto created = GenerateBiomeRegion(position);
				if (x == offset.X && y == offset.Y)
					newRegion = created;
			}
		}

		std::unordered_set<BiomeRegionOffsetVector> updatedRegions;
		for (int64_t y = offset.Y - radius; y <= offset.Y + radius; y++) {
			for (int64_t x = offset.X - radius; x <= offset.X + radius; x++) {
				auto position = BiomeRegionOffsetVector(x, y);
				auto updated = MergeRegion(GetBiomeRegionAt(position));
				updatedRegions.insert(position);
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
				std::shared_ptr<events::EBiomeRegionUpdate>(new events::EBiomeRegionUpdate(updatedVec)));
		}
		return newRegion;
	}

	std::shared_ptr<BiomeRegionData> BiomeRegionLoader::GetBiomeRegionAt(
		const BiomeRegionOffsetVector & offset
	) {
		//UE_LOG(LogTemp, Error, TEXT("Loading chunk at offset: %d %d %d"), offset.X, offset.Y, offset.Z);
		if (LoadedBiomeRegionCache.count(offset) > 0) {
			return LoadedBiomeRegionCache.at(offset);
		} else {
			if (IsBiomeRegionGenerated(offset))
				return LoadBiomeRegionFromDisk(offset);
			else
				return GenerateBiomeRegionArea(offset, 1);
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

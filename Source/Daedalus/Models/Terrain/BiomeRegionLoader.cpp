#include "Daedalus.h"
#include "BiomeRegionLoader.h"
#include "Hash.h"

#include <functional>
#include <random>

namespace terrain {
	BiomeRegionLoader::BiomeRegionLoader(const BiomeGeneratorParameters & params) {
		this->BiomeGenParams = params;
	}

	BiomeRegionLoader::~BiomeRegionLoader() {
		LoadedBiomeRegionCache.empty();
	}

	void BiomeRegionLoader::MergeRegionEdge(
		const BiomeRegionData & r1,
		const BiomeRegionData & r2
	) const {
		UE_LOG(LogTemp, Warning, TEXT("Merging edges: (%ld %ld) - (%ld %ld)"), r1.BiomeOffset.X, r1.BiomeOffset.Y, r2.BiomeOffset.X, r2.BiomeOffset.Y)
	}

	void BiomeRegionLoader::MergeRegionCorner(
		const BiomeRegionData & tl,
		const BiomeRegionData & tr,
		const BiomeRegionData & bl,
		const BiomeRegionData & br
	) const {
		UE_LOG(LogTemp, Warning, TEXT("Merging corners: (%ld %ld) - (%ld %ld) - (%ld %ld) - (%ld %ld)"),
			tl.BiomeOffset.X, tl.BiomeOffset.Y, tr.BiomeOffset.X, tr.BiomeOffset.Y,
			bl.BiomeOffset.X, bl.BiomeOffset.Y, br.BiomeOffset.X, br.BiomeOffset.Y)
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
		std::vector<utils::Vector2<> > vertexList;
		
		// Create uniform random point distribution, and insert vertices into aggregate list
		for (auto y = 0; y < BiomeGenParams.BiomeGridCellSize.Y; y++) {
			for (auto x = 0; x < BiomeGenParams.BiomeGridCellSize.X; x++) {
				numPoints = randNumPoints();
				offset.Reset(x, y);
				for (auto n = numPoints - 1; n >= 0; n--) {
					// Set point X, Y to random point within cell
					point.Reset(randPosition(), randPosition());
					data->InsertPoint(x, y, point);

					// Align X, Y relative to entire region
					point = (point + offset) / biomeRegionSize;
					vertexList.push_back(point);
				}
			}
		}

		utils::BuildDelaunay2D(data->DelaunayGraph, vertexList);

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
		neighbors.Set(1, 1, dataRef);

		// Merge shared edges
		for (int8 offY = -1; offY <= 1; offY++) {
			for (int8 offX = -1; offX <= 1; offX++) {
				uint8 dist = offY * offY + offX * offX;
				if (dist == 1) { // edge
					auto region = neighbors.Get(offX + 1, offY + 1);
					if (region.IsValid())
						MergeRegionEdge(*data, *region);
				} else if (dist == 2) { // corner
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
				LoadedBiomeRegionCache.insert({ offset, generated });
				return generated;
			}
		}
	}

	const BiomeGeneratorParameters & BiomeRegionLoader::GetGeneratorParameters() const {
		return BiomeGenParams;
	}
}

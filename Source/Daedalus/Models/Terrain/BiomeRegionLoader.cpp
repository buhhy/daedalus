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

	TSharedPtr<BiomeRegionData> BiomeRegionLoader::LoadBiomeRegionFromDisk(
		const BiomeOffsetVector & offset
	) {
		return TSharedPtr<BiomeRegionData>(NULL);
	}

	TSharedRef<BiomeRegionData> BiomeRegionLoader::GenerateMissingBiomeRegion(
		const BiomeOffsetVector & biomeOffset
	) {
		auto data = new BiomeRegionData(
			BiomeGenParams.BufferSize, BiomeGenParams.BiomeGridCellSize, biomeOffset);

		// Initialize Mersenne Twister PRNG with seed
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

		return TSharedRef<BiomeRegionData>(data);
	}

	TSharedRef<BiomeRegionData> BiomeRegionLoader::GetBiomeRegionAt(
		const BiomeOffsetVector & offset
	) {
		//UE_LOG(LogTemp, Error, TEXT("Loading chunk at offset: %d %d %d"), offset.X, offset.Y, offset.Z);
		if (LoadedBiomeRegionCache.count(offset) > 0) {
			return LoadedBiomeRegionCache.at(offset);
		} else {
			auto loaded = LoadBiomeRegionFromDisk(offset);
			if (loaded.IsValid())
				return loaded.ToSharedRef();
			return GenerateMissingBiomeRegion(offset);
		}
	}

	const BiomeGeneratorParameters & BiomeRegionLoader::GetGeneratorParameters() const {
		return BiomeGenParams;
	}
}
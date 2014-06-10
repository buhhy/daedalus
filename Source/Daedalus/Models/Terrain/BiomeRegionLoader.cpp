#include "Daedalus.h"
#include "BiomeRegionLoader.h"

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
		const BiomeOffsetVector & offset
	) {
		auto data = new BiomeRegionData(BiomeGenParams.BiomeGridCellSize, offset);
		return TSharedRef<BiomeRegionData>(data);
	}

	TSharedRef<BiomeRegionData> BiomeRegionLoader::GetChunkAt(
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

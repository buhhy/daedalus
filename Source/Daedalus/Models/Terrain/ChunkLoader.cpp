#include <Daedalus.h>
#include "ChunkLoader.h"

namespace terrain {
	using BiomeRegionLoaderPtr = ChunkLoader::BiomeRegionLoaderPtr;
	using ChunkCache = ChunkLoader::ChunkCache;

	ChunkLoader::~ChunkLoader() {
		LoadedChunkCache.empty();
	}

	ChunkDataPtr ChunkLoader::LoadChunkFromDisk(const ChunkOffsetVector & offset) {
		return std::shared_ptr<ChunkData>(NULL);
	}

	ChunkDataPtr ChunkLoader::GenerateMissingChunk(const ChunkOffsetVector & offset) {
		auto data = new ChunkData(TerrainGenParams.GridCellCount, offset);
		auto point = TerrainGenParams.ToRealCoordinates(offset);
		auto biomeTri = BRLoader->FindContainingBiomeTriangle(point);
		BRLoader->GetBiomeGenParams();
		UVWVector uvw = found.InterpolatePoint(globalPos);
		double height =
		found[1]->GetElevation() * uvw.X +
		found[2]->GetElevation() * uvw.Y +
		found[0]->GetElevation() * uvw.Z;
		Uint8 colour = (height / 2.0) * 255;
		//Uint8 colour = Uint8(found[0]->GetElevation() / 2.0 * 255);
		SDL_SetRenderDrawColor(Renderer.Renderer, colour, colour, colour, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(Renderer.Renderer, x, Renderer.Height - y);
		auto biome = BRLoader->GetBiomeAt(offset);
		SetDefaultHeight(*data, biome->GetElevation() * 250);
		return std::shared_ptr<ChunkData>(data);
	}

	ChunkDataPtr ChunkLoader::GetChunkAt(const ChunkOffsetVector & offset) {
		//UE_LOG(LogTemp, Error, TEXT("Loading chunk at offset: %d %d %d"), offset.X, offset.Y, offset.Z);
		if (LoadedChunkCache.count(offset) > 0) {
			return LoadedChunkCache.at(offset);
		} else {
			auto loaded = LoadChunkFromDisk(offset);
			return loaded ? loaded : GenerateMissingChunk(offset);
		}
	}

	const TerrainGeneratorParameters & ChunkLoader::GetGeneratorParameters() const {
		return TerrainGenParams;
	}

	void ChunkLoader::SetDefaultHeight(ChunkData & data, const Int32 height) {
		// TODO: if the chunk height ended on a chunk division line, no triangles are generated
		auto chunkHeight = TerrainGenParams.ChunkScale;
		if (((data.ChunkOffset.Z + 1) * (Int64) chunkHeight) < height) {
			data.DensityData.Fill(1.0);			// Completely filled block
			//UE_LOG(LogTemp, Error, TEXT("Ground chunk"));
		} else if ((data.ChunkOffset.Z * (Int64) chunkHeight) > height) {
			data.DensityData.Fill(0.0);			// Completely empty block
			//UE_LOG(LogTemp, Error, TEXT("Air chunk"));
		} else {
			//UE_LOG(LogTemp, Error, TEXT("Mixed chunk"));
			auto localHeight = TerrainGenParams.GridCellCount * (height / chunkHeight - data.ChunkOffset.Z);
			for (Uint32 x = 0; x < data.ChunkFieldSize; x++) {
				for (Uint32 y = 0; y < data.ChunkFieldSize; y++) {
					for (Uint32 z = 0; z < data.ChunkFieldSize && z < localHeight; z++)
						data.DensityData.Set(x, y, z, 1);
				}
			}
		}
	}
}

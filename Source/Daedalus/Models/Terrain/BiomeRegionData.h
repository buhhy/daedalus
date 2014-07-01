#pragma once

#include "TerrainDataStructures.h"
#include <Utilities/Graph/Delaunay.h>
#include <Utilities/DataStructures.h>

#include <array>
#include <vector>
#include <memory>
#include <unordered_map>

namespace terrain {
	typedef utils::Vector2<> BiomeCellVertex;
	typedef std::vector<uint64_t> PointIds;

	/**
	 * This class represents the actual biome itself along with along data associated with the
	 * biome such as elevation and rainfall, etc. Thousands of biomes of contained within each
	 * biome region. A biome is located at each point of the Delaunay triangulation and the
	 * boundaries are found by taking the Voronoi dual of the triangulation.
	 */
	class BiomeData {
	public:
		enum TerrainTypeId {
			T_PLAINS,
			T_MOUNTAINS
		};

		enum VegetationTypeId {
			V_BARREN,
			V_GRASSY,
			V_FOREST
		};

	private:
		BiomeId GlobalId;
		BiomeCellVertex LocalPosition;
		float Elevation;                        // Average biome elevation in metres
		float Rainfall;                         // Average rainfall in millimetres
		TerrainTypeId TerrainType;              // Topology of terrain
		VegetationTypeId VegetationType;        // General vegetation within terrain

	public:
		BiomeData(const BiomeId & gid, const BiomeCellVertex & position) :
			GlobalId(gid),
			LocalPosition(position),
			Elevation(0), Rainfall(0),
			TerrainType(T_PLAINS), VegetationType(V_BARREN)
		{}

		const BiomeCellVertex & GetLocalPosition() const { return LocalPosition; }
		const BiomeId & GlobalBiomeId() const { return GlobalId; }
	};

	typedef std::unordered_map<uint64_t, std::shared_ptr<BiomeData> > BiomeDataMap;

	/**
	 * Each biome cell contains a list of 2D points that will be used in a Delaunay
	 * triangulation. Because we need to merge Delaunay triangulations in each region,
	 * we need a buffer border around each region that isn't finalized until that buffer
	 * border has been used in an adjacent region merge.
	 */
	struct BiomeCellData {
		PointIds PointIds;

		BiomeCellData() {}

		~BiomeCellData() {}

		inline void AddPoint(const uint64_t id) { PointIds.push_back(id); }
	};

	typedef utils::Tensor2<BiomeCellData> BiomePointField;

	/**
	 * Biomes are generated on a 2D plane separate from the terrain generation. Biome shapes
	 * are created using a tiled Delaunay triangulation. Each biome region is generated
	 * at a larger scale than terrain chunks and will be subdivided evenly into many biome
	 * cells. Each biome cell contains a least 1 Delaunay point for more even triangulations.
	 */
	class BiomeRegionData {
	private:
		bool bIsGraphGenerated;
		bool bIsBiomesGenerated;

		uint64_t CurrentVertexId;
		BiomeDataMap Biomes;

		inline uint64_t GetNextId() { return CurrentVertexId++; }
		inline const BiomeId BuildId(uint64_t localId) {
			return BiomeId(BiomeOffset, localId);
		}

	public:
		BiomePointField PointDistribution;
		utils::DelaunayGraph DelaunayGraph;

		// Indicates whether the 8 neighboring regions have been generated yet, (0, 0) is
		// bottom left, (2, 2) is top right.
		utils::Tensor2<bool> NeighboursMerged;

		uint32_t BiomeGridSize;                     // Size of the biome in grid cells
		BiomeRegionOffsetVector BiomeOffset;      // Biome offset from (0,0)

		BiomeRegionData(
			const uint16_t buffer,
			const uint32_t biomeSize,
			const BiomeRegionOffsetVector & biomeOffset);

		~BiomeRegionData() {}


		inline BiomeData * GetBiomeAt(const uint64_t localBiomeId) {
			return Biomes.at(localBiomeId).get();
		}

		inline const BiomeData * GetBiomeAt(const uint64_t localBiomeId) const {
			return Biomes.at(localBiomeId).get();
		}

		inline bool AllNeighboursLoaded() const {
			bool done = true;
			for (uint8_t x = 0; x < NeighboursMerged.Width; x++) {
				for (uint8_t y = 0; y < NeighboursMerged.Depth; y++) {
					if (!NeighboursMerged.Get(x, y)) {
						done = false;
						break;
					}
				}
			}
			return done;
		}


		uint64_t AddBiome(const uint32_t x, const uint32_t y, const BiomeCellVertex & position);

		void GenerateDelaunayGraph();

		std::tuple<uint64_t, BiomeRegionGridVector, double> FindNearestPoint(
			utils::Vector2<> offset) const;
	};
}

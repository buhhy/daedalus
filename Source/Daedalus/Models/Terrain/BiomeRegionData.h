#pragma once

#include "TerrainDataStructures.h"
#include "DataStructures.h"
#include "Delaunay.h"

#include <array>
#include <vector>
#include <memory>
#include <unordered_map>

namespace terrain {
	typedef utils::Vector2<> BiomeCellVertex;
	typedef std::vector<uint64> PointIds;

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
			GlobalId(gid), LocalPosition(position) {}

		const BiomeCellVertex & GetLocalPosition() const { return LocalPosition; }
		const BiomeId & GlobalBiomeId() const { return GlobalId; }
	};

	typedef std::unordered_map<uint64, std::unique_ptr<BiomeData> > BiomeDataMap;

	/**
	 * Each biome cell contains a list of 2D points that will be used in a Delaunay
	 * triangulation. Because we need to merge Delaunay triangulations in each region,
	 * we need a buffer border around each region that isn't finalized until that buffer
	 * border has been used in an adjacent region merge.
	 */
	struct BiomeCellData {
		PointIds PointIds;
		bool IsFinalized;

		BiomeCellData(): IsFinalized(false) {}

		~BiomeCellData() {}

		inline void AddPoint(const uint64 id) { PointIds.push_back(id); }
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
		uint64 CurrentVertexId;
		BiomeDataMap Biomes;

		inline uint64 GetNextId() { return CurrentVertexId++; }
		inline const BiomeId BuildId(uint64 localId) {
			return BiomeId(BiomeOffset, localId);
		}

	public:
		BiomePointField PointDistribution;
		utils::DelaunayGraph DelaunayGraph;

		// Indicates whether the 8 neighboring regions have been generated yet, (0, 0) is
		// bottom left, (2, 2) is top right.
		utils::Tensor2<bool> NeighborsLoaded;

		uint32 BiomeGridSize;                     // Size of the biome in grid cells
		BiomeRegionOffsetVector BiomeOffset;      // Biome offset from (0,0)

		BiomeRegionData(
			const uint16 buffer,
			const uint32 biomeSize,
			const BiomeRegionOffsetVector & biomeOffset);

		~BiomeRegionData() {}


		inline BiomeData * GetBiomeAt(const uint64 localBiomeId) {
			return Biomes.at(localBiomeId).get();
		}

		inline const BiomeData * GetBiomeAt(const uint64 localBiomeId) const {
			return Biomes.at(localBiomeId).get();
		}


		uint64 AddBiome(const uint64 x, const uint64 y, const BiomeCellVertex & position);
		std::tuple<uint64, BiomeRegionGridVector, double> FindNearestPoint(
			utils::Vector2<> offset) const;
	};
}

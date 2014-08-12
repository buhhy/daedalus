#pragma once

#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <memory>

namespace items {
	enum ItemType {
		I_Chest,
		I_Sofa
	};

	struct ItemRotation {
		Uint8 Yaw, Pitch;
		ItemRotation(const Uint8 yaw, const Uint8 pitch) : Yaw(yaw), Pitch(pitch) {}

		void Bound(const ItemRotation & bound) {
			Yaw %= bound.Yaw;
			Pitch %= bound.Pitch;
		}

		void Add(const ItemRotation & rotation) {
			Yaw += rotation.Yaw;
			Pitch += rotation.Pitch;
		}
	};

	/**
	 * This class specifies the template for each item type. The template is used to initialize
	 * all the default values for a particular item data. This could be useful later when
	 * designing the modding system.
	 */
	struct ItemDataTemplate {
		ItemType Type;
		utils::Vector3D<> Size;  // Size in grid cells
		utils::Point3D Pivot;    // Position of pivot in grid cells
		ItemRotation RotationInterval;
		std::string MeshName;

		ItemDataTemplate(
			const ItemType type,
			const ItemRotation & rotInt,
			const utils::Vector3D<> & size,
			const utils::Point3D & pivot,
			const std::string & meshName
		) : Type(type),
			RotationInterval(rotInt),
			Pivot(pivot),
			Size(size),
			MeshName(meshName)
		{}
	};

	/**
	 * This data structure contains item-specific data. An item is some entity placed in the world
	 * that can be interacted with by players or AI. Items in the inventory should reference
	 * an item data structure.
	 * Items have discrete rotation about the vertical Z axis and the horizontal X axis. Not
	 * all items can be rotated about the X-axis. The interval variables define how many possible
	 * orientations are possible for each rotation axis. Each orientation angle size is equal.
	 */
	struct ItemData {
	private:
		ItemRotation Rotation;
		utils::AxisAlignedBoundingBox3D OriginBounds;

	public:
		Uint64 ItemId;
		terrain::ChunkPositionVector Position;
		bool bIsPlaced;
		// Size in grid cells, 1.0 is 1 grid cell, 0.5 is half a grid cell
		utils::Vector3D<> Size;
		const ItemDataTemplate & Template;

		ItemData(
			const Uint64 itemId,
			const terrain::ChunkPositionVector & position,
			const ItemRotation & rotation,
			const bool isPlaced,
			const ItemDataTemplate & tmp
		) : ItemId(itemId),
			Position(position),
			Rotation(rotation),
			bIsPlaced(isPlaced),
			Size(tmp.Size),
			Template(tmp),
			OriginBounds(0, tmp.Size)
		{}

		ItemData(const ItemDataTemplate & tmp) :
			ItemData(0, terrain::ChunkPositionVector(0, 0), ItemRotation(0, 0), false, tmp)
		{}

		const ItemRotation & GetRotation() const { return Rotation; }

		void SetRotation(const ItemRotation & rotation) {
			this->Rotation = rotation;
			this->Rotation.Bound(Template.RotationInterval);
		}

		void AddRotation(const ItemRotation & rotation) {
			this->Rotation.Add(rotation);
			this->Rotation.Bound(Template.RotationInterval);
		}
		
		utils::Matrix4D<> GetRotationMatrix() const {
			const double yawV = 360 * (double) Rotation.Yaw / Template.RotationInterval.Yaw;
			const double pitchV = 360 * (double) Rotation.Pitch / Template.RotationInterval.Pitch;

			return
				utils::CreateTranslation(Template.Pivot) *
				utils::CreateRotation(yawV, utils::AXIS_Z) *
				utils::CreateRotation(pitchV, utils::AXIS_X) *
				utils::CreateTranslation(-Template.Pivot);
		}

		utils::Matrix4D<> GetPositionMatrix() const {
			return utils::CreateTranslation(Position.second) * GetRotationMatrix();
		}
	};

	using ItemDataPtr = std::shared_ptr<ItemData>;
}

namespace std {
	template <> struct hash<items::ItemType> {
		size_t operator()(const items::ItemType & tp) const {
			return std::hash<long>()(tp);
		}
	};
}


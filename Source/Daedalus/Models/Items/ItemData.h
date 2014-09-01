#pragma once

#include <Models/Terrain/TerrainDataStructures.h>
#include <Utilities/Algebra/Algebra3D.h>

#include <memory>
#include <unordered_set>

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

	bool operator == (const ItemRotation & lhs, const ItemRotation & rhs);
	bool operator != (const ItemRotation & lhs, const ItemRotation & rhs);

	/**
	 * This class specifies the template for each item type. The template is used to initialize
	 * all the default values for a particular item data. This could be useful later when
	 * designing the modding system.
	 */
	struct ItemDataTemplate {
		ItemType Type;
		// Bounds of the item in grid cells.
		utils::AxisAlignedBoundingBox3D ItemBounds;
		// Position of pivot in grid cells.
		utils::Point3D Pivot;
		ItemRotation RotationInterval;
		std::string MeshName;
		// Maximum number of items possible in an item stack.
		Uint32 MaxStackSize;
		// Maximum number of concurrent users of this item.
		utils::Option<Uint16> maxCurrentUsers;
		float tickDuration;

		ItemDataTemplate(
			const ItemType type,
			const ItemRotation & rotInt,
			const utils::AxisAlignedBoundingBox3D & bounds,
			const utils::Point3D & pivot,
			const std::string & meshName,
			const Uint32 maxStackSize,
			const utils::Option<Uint16> maxCurUsers = utils::None<Uint16>()
		) : Type(type),
			RotationInterval(rotInt),
			Pivot(pivot),
			ItemBounds(bounds),
			MeshName(meshName),
			MaxStackSize(maxStackSize),
			maxCurrentUsers(maxCurUsers),
			tickDuration(20)
		{}
	};

	struct ItemDataId {
		Uint64 ItemId;
		terrain::ChunkOffsetVector ChunkOffset;

		ItemDataId(Uint64 id, terrain::ChunkOffsetVector offset) :
			ItemId(id), ChunkOffset(offset)
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
	class ItemData {
	private:
		using ItemUsersSet = std::unordered_set<Uint64>;

		ItemRotation Rotation;
		// Size in grid cells, 1.0 is 1 grid cell, 0.5 is half a grid cell. The reason this exists
		// here as well as in the templates is to allow individually scaled objects.
		utils::AxisAlignedBoundingBox3D ItemBounds;
		ItemUsersSet currentItemUsers;

	public:
		Uint64 ItemId;
		terrain::ChunkPositionVector Position;
		bool bIsPlaced;
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
			Template(tmp),
			ItemBounds(tmp.ItemBounds)
		{}

		ItemData(const ItemDataTemplate & tmp) :
			ItemData(0, terrain::ChunkPositionVector(), ItemRotation(0, 0), false, tmp)
		{}

		const ItemRotation & getRotation() const { return Rotation; }
		const terrain::ChunkPositionVector & getPosition() const { return Position; }

		void setPosition(const terrain::ChunkPositionVector & position) {
			this->Position = position;
		}

		void SetRotation(const ItemRotation & rotation) {
			this->Rotation = rotation;
			this->Rotation.Bound(Template.RotationInterval);
		}

		void AddRotation(const ItemRotation & rotation) {
			this->Rotation.Add(rotation);
			this->Rotation.Bound(Template.RotationInterval);
		}

		utils::Matrix4D<> GetPositionMatrix() const {
			return utils::CreateTranslation(Position.InnerOffset) * GetRotationMatrix();
		}

		utils::OrientedBoundingBox3D GetBoundingBox() const {
			return utils::OrientedBoundingBox3D(ItemBounds, GetPositionMatrix());
		}
		
		ItemDataId getItemId() const { return ItemDataId(ItemId, Position.ChunkOffset); }
		
		utils::Matrix4D<> GetRotationMatrix() const;
		bool addUser(const Uint64 charId);
		bool removeUser(const Uint64 charId);
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


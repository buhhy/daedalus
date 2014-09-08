#include <Daedalus.h>
#include "ItemData.h"

#include <Models/Fauna/CharacterData.h>

namespace items {
	using namespace utils;
	using namespace fauna;

	bool operator == (const ItemRotation & lhs, const ItemRotation & rhs) {
		return lhs.Yaw == rhs.Yaw && lhs.Pitch == rhs.Pitch;
	}

	bool operator != (const ItemRotation & lhs, const ItemRotation & rhs) {
		return !(lhs == rhs);
	}

	Matrix4D<> ItemData::GetPositionMatrix() const {
		return CreateTranslation(position.InnerOffset) * GetRotationMatrix();
	}

	Matrix4D<> ItemData::GetRotationMatrix() const {
		const double yawV = 360 * (double) rotation.Yaw / Template.RotationInterval.Yaw;
		const double pitchV = 360 * (double) rotation.Pitch / Template.RotationInterval.Pitch;

		return
			CreateTranslation(Template.Pivot) *
			CreateRotation(yawV, utils::AXIS_Z) *
			CreateRotation(pitchV, utils::AXIS_X) *
			CreateTranslation(-Template.Pivot);
	}
	
	Matrix4D<> ItemData::getScaleMatrix() const {
		return CreateScaling(scale);
	}

	bool ItemData::addUser(const Uint64 charId) {
		if (currentItemUsers.find(charId) != currentItemUsers.end())
			return false;

		// No limit on number of users.
		if (!Template.maxCurrentUsers.IsValid() ||
				(*Template.maxCurrentUsers) > currentItemUsers.size())
		{
			currentItemUsers.insert(charId);
			return true;
		}

		return false;
	}

	bool ItemData::removeUser(const Uint64 charId) {
		if (currentItemUsers.find(charId) == currentItemUsers.end())
			return false;

		currentItemUsers.erase(charId);
		return true;
	}

	void ItemData::interactAction(CharacterDataPtr & charData) {
		auto self = shared_from_this();
		charData->startUsingItem(self);
	}
}
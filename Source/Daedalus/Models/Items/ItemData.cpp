#include <Daedalus.h>
#include "ItemData.h"

namespace items {
	using namespace utils;

	bool operator == (const ItemRotation & lhs, const ItemRotation & rhs) {
		return lhs.Yaw == rhs.Yaw && lhs.Pitch == rhs.Pitch;
	}

	bool operator != (const ItemRotation & lhs, const ItemRotation & rhs) {
		return !(lhs == rhs);
	}

	Matrix4D<> ItemData::GetRotationMatrix() const {
		const double yawV = 360 * (double) Rotation.Yaw / Template.RotationInterval.Yaw;
		const double pitchV = 360 * (double) Rotation.Pitch / Template.RotationInterval.Pitch;

		return
			CreateTranslation(Template.Pivot) *
			CreateRotation(yawV, utils::AXIS_Z) *
			CreateRotation(pitchV, utils::AXIS_X) *
			CreateTranslation(-Template.Pivot);
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

		currentItemUsers.remove(charId);
		return true;
	}
}
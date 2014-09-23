#pragma once

#include <Utilities/DataStructures.h>

#include <string>
#include <memory>

namespace fauna {
	class CharacterData;
	using CharacterDataPtr = std::shared_ptr<CharacterData>;
}

/**
 * All items, spells and interactables that can be added to the shortcut bar should inherit
 * from this interface.
 */
class IShortcut {
public:
	virtual void useShortcut(fauna::CharacterDataPtr & charData) = 0;
	virtual std::string getIconName() const = 0;
	virtual utils::Option<Uint32> getQuantity() const = 0;
};

using ShortcutPtr = std::shared_ptr<IShortcut>;

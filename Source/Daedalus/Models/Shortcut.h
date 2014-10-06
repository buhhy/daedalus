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
class IQuickuse {
public:
	virtual void quickuse(const fauna::CharacterDataPtr & charData) = 0;
	virtual std::string getIconName() const = 0;
	virtual utils::Option<Uint32> getQuantity() const = 0;
	virtual bool isValid() const = 0;
};

using IQuickusePtr = std::shared_ptr<IQuickuse>;
using IQuickuseCPtr = std::shared_ptr<const IQuickuse>;

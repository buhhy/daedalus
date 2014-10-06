#pragma once

#include <Utilities/DataStructures.h>

#include "ControllableCharacter.generated.h"

UINTERFACE(MinimalAPI)
class UControllableCharacter : public UInterface {
	GENERATED_UINTERFACE_BODY()
};

/**
 * Abstract class that provides a consistent set of public-facing control APIs for all
 * player controllable characters.
 */
class IControllableCharacter {
	GENERATED_IINTERFACE_BODY()
public:
	// Control methods
	virtual void moveForward(float amount) = 0;
	virtual void moveRight(float amount) = 0;
	virtual void lookUp(float amount) = 0;
	virtual void lookRight(float amount) = 0;
	virtual void quickuse(const Uint8 slot) = 0;
	virtual void holdJump() = 0;
	virtual void releaseJump() = 0;
	virtual void rightMouseDown() = 0;
	virtual void rightMouseUp() = 0;
	virtual void leftMouseDown() = 0;
	virtual void leftMouseUp() = 0;
	virtual void escapeKey() = 0;
	virtual void toggleHandAction() = 0;
};

#pragma once

#include <Utilities/Algebra/Algebra2D.h>

namespace gui {
	/**
	 * An element to be displayed on the HUD that contains convenience methods for hitbox
	 * detection and event handling.
	 */
	class HUDElement {
	protected:
		virtual utils::AxisAlignedBoundingBox2D getBounds() const = 0;
	public:
		virtual bool hitTest(const utils::Point2D & pointerPos);
		virtual void tick(float delta);
		virtual void onMouseOver(const utils::Point2D & position);
		virtual void onMouseDown(const Uint8 whichBtn);
		virtual void onMouseUp(const Uint8 whichBtn);
	};
}
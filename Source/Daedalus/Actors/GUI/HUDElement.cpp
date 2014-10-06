#include <Daedalus.h>
#include <Actors/GUI/HUDElement.h>
#include <Actors/GUI/PlayerHUD.h>

namespace gui {
	using namespace utils;
	using namespace fauna;
	
	/********************************************************************************
	 * HUDElement
	 ********************************************************************************/

	HUDElement::HUDElement(const utils::Point2D & origin, const utils::Point2D & size) :
		bounds(origin, size), bIsMouseInside(false), bIsHidden(false)
	{}

	HUDElement::HUDElement() : HUDElement({ 0, 0 }, { 0, 0 }) {}

	bool HUDElement::isMouseInside() const {
		return bIsMouseInside;
	}

	void HUDElement::tick() {}
	void HUDElement::drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) {}
	
	void HUDElement::onMouseMove(const utils::Point2D & position) {}

	bool HUDElement::onMouseOver(const utils::Point2D & position) {
		return true; // Should propagate
	}
	
	bool HUDElement::onMouseLeave(const utils::Point2D & position) {
		return true; // Should propagate
	}

	bool HUDElement::onMouseDown(const MouseEvent & evt) {
		return true; // Should propagate
	}

	bool HUDElement::onMouseUp(const MouseEvent & evt) {
		return true; // Should propagate
	}

	void HUDElement::preMouseDown(const MouseEvent & evt) {}
	void HUDElement::preMouseUp(const MouseEvent & evt) {}

	void HUDElement::attachTo(const HUDElementPtr & node) {
		parent = node;
	}

	bool HUDElement::appendChild(const HUDElementPtr & child) {
		children.push_back(child);
		child->attachTo(shared_from_this());
		return true;
	}

	HUDElementPtr HUDElement::removeChild(const Uint32 index) {
		if (index >= childCount())
			return nullptr;
		return *(children.erase(children.begin() + index));
	}
	
	HUDElementPtr HUDElement::getChild(const Uint32 index) {
		if (index >= childCount())
			return nullptr;
		return children[index];
	}

	HUDElementPtr HUDElement::getParent() {
		return parent;
	}

	Uint32 HUDElement::childCount() const {
		return children.size();
	}

	bool HUDElement::hitTest(const utils::Point2D & pointerPos) {
		return getBounds().isInside(pointerPos);
	}

	const utils::Box2D & HUDElement::getBounds() const {
		return bounds;
	}

	Point2D HUDElement::getAbsolutePosition() const {
		if (parent)
			return parent->getAbsolutePosition() + bounds.origin;
		return bounds.origin;
	}
	
	void HUDElement::drawElementTree(APlayerHUD * hud, const ResourceCacheCPtr & rcache) {
		if (!isHidden()) {
			drawElement(hud, rcache);
			for (auto & child : children)
				child->drawElementTree(hud, rcache);
		}
	}

	void HUDElement::runLogic(const float delta) {
		tick();
		for (auto & child : children)
			child->runLogic(delta);
	}
	
	bool HUDElement::checkMouseMove(const utils::Point2D & position) {
		for (auto & child : children) {
			if (!child->checkMouseMove(position))
				return false;
		}

		bool isInside = hitTest(position);
		if (isInside && !bIsMouseInside) {
			bIsMouseInside = true;
			return onMouseOver(position);
		} else if (!isInside && bIsMouseInside) {
			bIsMouseInside = false;
			return onMouseLeave(position);
		} else {
			onMouseMove(position);
		}

		return true;
	}

	bool HUDElement::checkMouseDown(const MouseEvent & evt) {
		for (auto & child : children) {
			if (!child->checkMouseDown(evt))
				return false;
		}

		bool isInside = hitTest(evt.position);
		if (isInside) {
			preMouseDown(evt);
			return onMouseDown(evt);
		}

		return true;
	}

	bool HUDElement::checkMouseUp(const MouseEvent & evt) {
		for (auto & child : children) {
			if (!child->checkMouseUp(evt))
				return false;
		}

		bool isInside = hitTest(evt.position);
		if (isInside) {
			preMouseUp(evt);
			return onMouseUp(evt);
		}

		return true;
	}

	void HUDElement::resize(const utils::Point2D & newSize) {
		bounds.size = newSize;
	}

	void HUDElement::reposition(const utils::Point2D & newPos) {
		bounds.origin = newPos;
	}

	void HUDElement::setHidden(const bool isHidden) {
		bIsHidden = isHidden;
	}

	bool HUDElement::isHidden() const {
		return bIsHidden;
	}

	
	
	/********************************************************************************
	 * CursorElement
	 ********************************************************************************/
	
	void CursorElement::tick() {
		if (parent)
			resize(parent->getBounds().size);
	}

	void CursorElement::drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) {
		std::string cursorSuffix, cursorPrefix;
		if (mouseButtonsActive & MouseEvent::BUTTON_PRESS_LEFT)
			cursorSuffix = "-Active";

		switch (currentCursorType) {
		case MouseEvent::C_Hover:
			cursorPrefix = "Pointer";
			break;
		case MouseEvent::C_Active:
			cursorPrefix = "Pointer";
			break;
		default:
			cursorPrefix = "Pointer";
			break;
		}

		UTexture2D * cursorTexture = rcache->findIcon(
			cursorPrefix + cursorSuffix, ResourceCache::ICON_CURSOR_FOLDER);

		hud->DrawTextureSimple(cursorTexture, cursorPosition.X, cursorPosition.Y);
	}

	void CursorElement::onMouseMove(const utils::Point2D & position) {
		cursorPosition = position;
	}

	bool CursorElement::onMouseDown(const MouseEvent & evt) {
		cursorPosition = evt.position;
		mouseButtonsActive = evt.activeButtons;
		return true;
	}

	bool CursorElement::onMouseUp(const MouseEvent & evt) {
		cursorPosition = evt.position;
		mouseButtonsActive = evt.activeButtons;
		return true;
	}
}

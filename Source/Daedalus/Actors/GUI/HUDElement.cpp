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
	
	HUDElementPtr HUDElement::clone() const {
		return nullptr;
	};

	bool HUDElement::isMouseInside() const {
		return bIsMouseInside;
	}

	void HUDElement::copyProperties(const HUDElementPtr & element) const {
		element->setHidden(bIsHidden);
		element->resize(bounds.size);
		element->reposition(bounds.origin);

		for (const auto & child : children)
			element->appendChild(child->clone());
	}
	
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

	bool HUDElement::onMouseUp(const MouseEvent & evt, const bool isInside) {
		return true; // Should propagate
	}

	void HUDElement::preMouseDown(const MouseEvent & evt) {}
	void HUDElement::preMouseUp(const MouseEvent & evt, const bool isInside) {}

	void HUDElement::onAttach(const HUDElementPtr & newParent) {}
	void HUDElement::onDetach(const HUDElementPtr & oldParent) {}

	void HUDElement::attachTo(const HUDElementPtr & node) {
		if (node)
			node->appendChild(shared_from_this());
	}
	
	void HUDElement::detach() {
		const auto lockedParent = parent.lock();
		if (lockedParent)
			lockedParent->removeChild(shared_from_this());
	}

	void HUDElement::appendChild(const HUDElementPtr & child) {
		if (child) {
			// If this child already exists in the children list, then remove it, and
			// re-append to the end of the children list. Otherwise, we need to detach
			// it from its existing parent.
			child->detach();
			children.push_back(child);
			child->parent = shared_from_this();
			child->onAttach(shared_from_this());
		}
	}

	void HUDElement::insertChild(const HUDElementPtr & child, const Uint32 index) {
		if (child) {
			// If this child already exists in the children list, then remove it, and
			// re-append to the end of the children list. Otherwise, we need to detach
			// it from its existing parent.
			child->detach();
			children.insert(children.cbegin() + index, child);
			child->parent = shared_from_this();
			child->onAttach(shared_from_this());
		}
	}

	bool HUDElement::removeChild(const HUDElementPtr & possibleChild) {
		bool removed = false;
		for (Uint32 i = 0; i < childCount(); i++) {
			if (getChild(i) == possibleChild) {
				removeChild(i);
				removed = true;
			}
		}

		return removed;
	}

	HUDElementPtr HUDElement::removeChild(const Uint32 index) {
		if (index >= childCount())
			return nullptr;
		auto removed = children[index];
		children.erase(children.begin() + index);
		// Detach this child from any existing parents.
		removed->parent.reset();
		removed->onDetach(shared_from_this());
		return removed;
	}
	
	HUDElementPtr HUDElement::getChild(const Uint32 index) {
		if (index >= childCount())
			return nullptr;
		return children[index];
	}
	
	HUDElementCPtr HUDElement::getChild(const Uint32 index) const {
		if (index >= childCount())
			return nullptr;
		return children[index];
	}

	HUDElementWPtr HUDElement::getParent() {
		return parent;
	}
	
	Option<Uint32> HUDElement::findChildIndex(const HUDElementPtr & possibleChild) const {
		for (Uint32 i = 0; i < childCount(); i++) {
			if (getChild(i) == possibleChild)
				return Some(i);
		}
		return None<Uint32>();
	}

	Uint32 HUDElement::childCount() const {
		return children.size();
	}

	bool HUDElement::hitTest(const utils::Point2D & pointerPos) {
		return getAbsoluteBounds().isInside(pointerPos);
	}

	const utils::Box2D & HUDElement::getBounds() const {
		return bounds;
	}

	Point2D HUDElement::getAbsolutePosition() const {
		const auto lockedParent = parent.lock();
		if (lockedParent)
			return lockedParent->getAbsolutePosition() + bounds.origin;
		return bounds.origin;
	}

	Box2D HUDElement::getAbsoluteBounds() const {
		// TODO: account for actual transforms, not just translation
		return Box2D(getAbsolutePosition(), getBounds().size);
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
		for (Uint32 i = 0; i < childCount(); i++) {
			if (!getChild(i)->checkMouseDown(evt))
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
		for (Uint32 i = 0; i < childCount(); i++) {
			if (!getChild(i)->checkMouseUp(evt))
				return false;
		}

		bool isInside = hitTest(evt.position);
		preMouseUp(evt, isInside);
		return onMouseUp(evt, isInside);
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
	 * DivElement
	 ********************************************************************************/

	DivElement::DivElement(const utils::Point2D & origin, const utils::Point2D & size) :
		HUDElement(origin, size)
	{}

	DivElement::DivElement() : DivElement({ 0, 0 }, { 0, 0 }) {}

	DivElement * DivElement::createNew() const {
		return new DivElement();
	}
	void DivElement::tick() {}
	void DivElement::drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) {}
	
	DivElementPtr DivElement::clone() const {
		const auto ret = DivElementPtr(createNew());
		copyProperties(ret);
		return ret;
	}

	
	
	/********************************************************************************
	 * CursorElement
	 ********************************************************************************/

	CursorElement::CursorElement() :
		previousCursorPosition(0, 0),
		currentCursorPosition(0, 0),
		mouseButtonsActive(0),
		currentCursorType(MouseEvent::C_Pointer),
		previousCursorType(MouseEvent::C_Pointer)
	{}

	CursorElement * CursorElement::createNew() const {
		return new CursorElement();
	}

	void CursorElement::tick() {
		const auto lockedParent = parent.lock();
		if (lockedParent)
			resize(lockedParent->getBounds().size);
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

		hud->DrawTextureSimple(
			cursorTexture, currentCursorPosition.X, currentCursorPosition.Y);
	}

	void CursorElement::onMouseMove(const utils::Point2D & position) {
		setCursorPosition(position);
	}

	bool CursorElement::onMouseDown(const MouseEvent & evt) {
		setCursorPosition(evt.position);
		mouseButtonsActive |= evt.whichButtons;
		return true;
	}

	bool CursorElement::onMouseUp(const MouseEvent & evt, const bool isInside) {
		setCursorPosition(evt.position);
		mouseButtonsActive ^= evt.whichButtons;
		return true;
	}
	
	void CursorElement::setCursorPosition(const utils::Point2D & pos) {
		previousCursorPosition = currentCursorPosition;
		currentCursorPosition = pos;
		const auto delta = currentCursorPosition - previousCursorPosition;

		// Reposition children.
		if (childCount() > 0 && !EEq(delta.Length2(), 0)) {
			for (const auto child : draggingElements) {
				const auto newPos = child->getBounds().origin + delta;
				child->reposition(newPos);
			}
		}
	}

	CursorElementPtr CursorElement::clone() const {
		const auto ret = CursorElementPtr(createNew());
		copyProperties(ret);
		return ret;
	}

	void CursorElement::startDragElement(const DragHolderElementPtr & element) {
		draggingElements.insert(element);
		appendChild(element);
	}

	void CursorElement::stopDragElement(const DragHolderElementPtr & element) {
		draggingElements.erase(element);
		removeChild(element);
	}

	const CursorElement::DraggableElementList & CursorElement::getDraggingElements() const {
		return draggingElements;
	}
}

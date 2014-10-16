#include <Daedalus.h>
#include <Actors/GUI/CursorElement.h>

#include <Actors/GUI/PlayerHUD.h>

namespace gui {
	using namespace utils;
	
	/********************************************************************************
	 * DragHolderElement
	 ********************************************************************************/

	DragHolderElement * DragHolderElement::createNew() const {
		return new DragHolderElement(creatorRef, placeholderRef, cursorRef);
	}

	void DragHolderElement::tick() {}

	void DragHolderElement::drawElement(
		APlayerHUD * hud,
		const ResourceCacheCPtr & rcache
	) {
	}

	/**
	 * @param creator The original creator source of this drag holder element, the holder
	 *                element inherits its starting position from this provided element,
	 *                if this element pointer is expired, then the position is determined
	 *                from the mouse cursor.
	 */
	DragHolderElement::DragHolderElement(
		DraggableElementWPtr creator,
		const HUDElementPtr & placeholder,
		const CursorElementPtr & cursor
	) : creatorRef(creator), placeholderRef(placeholder), cursorRef(cursor)
	{
		const auto lockedCreator = creator.lock();
		if (lockedCreator)
			reposition(lockedCreator->getAbsolutePosition());
		else
			reposition(cursorRef->getAbsolutePosition());

		placeholderRef->detach();
		placeholderRef->reposition({ 0, 0 });
	}
	
	void DragHolderElement::onAttach(const HUDElementPtr & newParent) {
		placeholderRef->attachTo(shared_from_this());
	}

	HUDElementPtr DragHolderElement::getContainedElement() {
		return placeholderRef;
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

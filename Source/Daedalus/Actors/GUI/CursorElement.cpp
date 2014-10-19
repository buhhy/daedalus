#include <Daedalus.h>
#include <Actors/GUI/CursorElement.h>
#include <Actors/GUI/Draggable.h>
#include <Actors/GUI/Droppable.h>

#include <Actors/GUI/PlayerHUD.h>

namespace gui {
	using namespace utils;
	
	/********************************************************************************
	 * DragHolderElement
	 ********************************************************************************/

	void DragHolderElement::initializeElement(const Point2D & cursorPosition) {
		const auto lockedParent = placeholderRef->getParent().lock();
		if (lockedParent) {
			const auto findex = lockedParent->findChildIndex(placeholderRef);
			assert(findex.IsValid() &&
				"DragHolderElement::initializeElement: Child element not found in parent's children array.");
			offsetPosition = placeholderRef->getAbsolutePosition() - cursorPosition;
			originalParentRef = lockedParent;
			originalPosition = placeholderRef->getBounds().origin;
			originalChildIndex = *findex;
		} else {
			offsetPosition = cursorPosition;
			originalParentRef.reset();
		}

		placeholderRef->detach();
		placeholderRef->reposition({ 0, 0 });
		onCursorReposition(cursorPosition);
	}

	void DragHolderElement::revert() {
		const auto lockedParent = originalParentRef.lock();
		if (lockedParent) {
			// On rejection, return element to original position if parent exists.
			lockedParent->insertChild(placeholderRef, originalChildIndex);
			placeholderRef->reposition(originalPosition);
		} else {
			// Otherwise, delete element.
			placeholderRef->detach();
		}
	}

	DragHolderElement * DragHolderElement::createNew() const {
		return new DragHolderElement(placeholderRef, cursorRef, offsetPosition);
	}

	void DragHolderElement::tick() {}

	void DragHolderElement::drawElement(
		APlayerHUD * hud,
		const ResourceCacheCPtr & rcache
	) {
	}

	DragHolderElement::DragHolderElement(
		const DraggableElementPtr & placeholder,
		const CursorElementPtr & cursor
	) : placeholderRef(placeholder), cursorRef(cursor)
	{
		initializeElement(cursor->getCursorPosition());
	}
	
	DragHolderElement::DragHolderElement(
		const DraggableElementPtr & placeholder,
		const CursorElementPtr & cursor,
		const Point2D & elementOffsetOverride
	) : placeholderRef(placeholder), cursorRef(cursor) {
		initializeElement(cursor->getCursorPosition());
		offsetPosition = elementOffsetOverride;
	}
	
	void DragHolderElement::onAttach(const HUDElementPtr & newParent) {
		placeholderRef->attachTo(shared_from_this());
	}

	bool DragHolderElement::onDragAccept(const Point2D & cursorPosition) {
		placeholderRef->onDragAccept(cursorPosition);
		bool retVal = false;

		switch (placeholderRef->getParameters().revertAction) {
		case DraggableParameters::RA_RevertAll:
			revert();
			retVal = true;
			break;
		default:
			break;
		}
		return retVal;
	}

	bool DragHolderElement::onDragReject(const Point2D & cursorPosition) {
		placeholderRef->onDragReject(cursorPosition);
		bool retVal = false;

		switch (placeholderRef->getParameters().revertAction) {
		case DraggableParameters::RA_RevertAll:
		case DraggableParameters::RA_RevertInvalid:
			revert();
			retVal = true;
			break;
		default:
			break;
		}
		return retVal;
	}

	DraggableElementPtr DragHolderElement::getContainedElement() {
		return placeholderRef;
	}

	void DragHolderElement::onCursorReposition(const Point2D & cursorPosition) {
		reposition(cursorPosition + offsetPosition);
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

	CursorElementPtr CursorElement::clone() const {
		const auto ret = CursorElementPtr(createNew());
		copyProperties(ret);
		return ret;
	}

	void CursorElement::addDragElement(const DragHolderElementPtr & element) {
		draggingElements.insert(element);
		appendChild(element);
	}

	void CursorElement::removeDragElement(const DragHolderElementPtr & element) {
		draggingElements.erase(element);
		removeChild(element);
	}

	void CursorElement::clearDragElements() {
		for (auto element : draggingElements)
			removeChild(element);
		draggingElements.clear();
	}

	const CursorElement::DraggableElementList & CursorElement::getDraggingElements() const {
		return draggingElements;
	}
	
	void CursorElement::setCursorPosition(const Point2D & pos) {
		previousCursorPosition = currentCursorPosition;
		currentCursorPosition = pos;
		const auto delta = currentCursorPosition - previousCursorPosition;

		// Reposition children.
		if (childCount() > 0 && !EEq(delta.Length2(), 0)) {
			for (const auto child : draggingElements) {
				const auto newPos = child->getBounds().origin + delta;
				child->onCursorReposition(currentCursorPosition);
			}
		}
	}
	
	const Point2D & CursorElement::getCursorPosition() const {
		return currentCursorPosition;
	}
}

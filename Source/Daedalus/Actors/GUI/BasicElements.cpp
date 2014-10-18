#include <Daedalus.h>
#include <Actors/GUI/BasicElements.h>
#include <Actors/GUI/PlayerHUD.h>

namespace gui {
	using namespace utils;

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
	 * DocumentRootElement
	 ********************************************************************************/

	DocumentRootElement::DocumentRootElement(const CursorElementPtr & cursor) :
		DroppableElement(DroppableParameters(), cursor)
	{}

	DocumentRootElement * DocumentRootElement::createNew() const {
		return new DocumentRootElement(this->cursorRef);
	}
	void DocumentRootElement::tick() {}
	void DocumentRootElement::drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) {}
	
	DocumentRootElementPtr DocumentRootElement::clone() const {
		const auto ret = DocumentRootElementPtr(createNew());
		copyProperties(ret);
		return ret;
	}

	bool DocumentRootElement::onDrop(
		const HUDElementPtr & draggable,
		const Point2D & position
	) {
		// By default, the element should be dropped off where the mouse stopped dragging.
		draggable->reposition(draggable->getAbsolutePosition());
		appendChild(draggable);
		return true;
	}
}

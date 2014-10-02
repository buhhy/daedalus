#include <Daedalus.h>
#include <Actors/GUI/HUDElement.h>

namespace gui {
	using namespace utils;

	HUDElement::HUDElement(const utils::Point2D & origin, const utils::Point2D & size) :
		bIsMouseInside(false), bounds(origin, size)
	{}

	HUDElement::HUDElement() : HUDElement({ 0, 0 }, { 0, 0 }) {}

	bool HUDElement::isMouseInside() const {
		return bIsMouseInside;
	}

	void HUDElement::tick() {}
	void HUDElement::drawElement(AHUD * hud, const ResourceCacheCPtr & rcache) {}

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
	
	void HUDElement::drawElementTree(AHUD * hud, const ResourceCacheCPtr & rcache) {
		drawElement(hud, rcache);
		for (auto & child : children)
			child->drawElementTree(hud, rcache);
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
		}

		return true;
	}

	bool HUDElement::checkMouseDown(const MouseEvent & evt) {
		for (auto & child : children) {
			if (!child->checkMouseDown(evt))
				return false;
		}

		bool isInside = hitTest(evt.position);
		if (isInside)
			return onMouseDown(evt);

		return true;
	}

	bool HUDElement::checkMouseUp(const MouseEvent & evt) {
		for (auto & child : children) {
			if (!child->checkMouseUp(evt))
				return false;
		}

		bool isInside = hitTest(evt.position);
		if (isInside)
			return onMouseUp(evt);

		return true;
	}

	void HUDElement::resize(const utils::Point2D & newSize) {
		bounds.size = newSize;
	}

	void HUDElement::reposition(const utils::Point2D & newPos) {
		bounds.origin = newPos;
	}



	/********************************************************************************
	 *
	 ********************************************************************************/

	Quickbar::Quickbar(const Point2D & origin, const Point2D & size) :
		HUDElement(origin, size), cachedQuickbarSize(0)
	{}
	
	Quickbar::Quickbar() : Quickbar({ 0, 0 }, { 0, 0 }) {}

	void Quickbar::tick() {
		const double slotSize = 64, slotBorder = 4;
		const double barHeight = slotSize + slotBorder * 2;
		bool hasChanged = false;

		// Check for quickbar size changes.
		Uint32 newSize = 0;
		if (model) {
			newSize = model->getMaxSize();

			for (; newSize > cachedQuickbarSize; cachedQuickbarSize++) {
				appendChild(HUDElementPtr(
					new QuickbarElement({ 0, 0 }, { slotSize, slotSize },
					(*model)[cachedQuickbarSize])));
				hasChanged = true;
			}
		}

		for (; cachedQuickbarSize > newSize; cachedQuickbarSize--) {
			removeChild(cachedQuickbarSize);
			hasChanged = true;
		}

		// Update size.
		if (parent) {
			const auto & parentBounds = parent->getBounds();

			resize(Point2D(parentBounds.size.X, barHeight));
			reposition(Point2D(0, parentBounds.size.Y - barHeight));

			if (hasChanged) {
				// Center the quick bar on the screen.
				const auto size = children.size();
				for (Uint32 i = 0; i < size; i++) {
					float x = bounds.size.X / 2 + (i - size / 2.0) *
						(slotSize + slotBorder) + slotBorder / 2;
					float y = bounds.size.Y - (slotSize + slotBorder);
					children[i]->reposition({ x, y });
				}
			}
		}
	}

	void Quickbar::drawElement(AHUD * hud, const ResourceCacheCPtr & rcache) {}
		
	bool Quickbar::onMouseOver(const Point2D & position) { return true; }
	bool Quickbar::onMouseLeave(const Point2D & position) { return true; }
	bool Quickbar::onMouseDown(const MouseEvent & evt) { return true; }
	bool Quickbar::onMouseUp(const MouseEvent & evt) { return true; }

	void Quickbar::updateQuickbar(const fauna::ShortcutBarPtr & bar) {
		model = bar;
	}



	/********************************************************************************
	 *
	 ********************************************************************************/

	QuickbarElement::QuickbarElement(
		const Point2D & origin,
		const Point2D & size,
		const ShortcutPtr & model
	) : HUDElement(origin, size), model(model)
	{}

	void QuickbarElement::tick() {}

	void QuickbarElement::drawElement(AHUD * hud, const ResourceCacheCPtr & rcache) {
		const auto & size = getBounds().size;
		const auto position = getAbsolutePosition();

		if (model && model->isValid()) {
			auto icon = rcache->findIcon(model->getIconName());
			auto font = rcache->findFont("Lato", 12);
			const auto count = model->getQuantity();
			hud->DrawTexture(
				icon, position.X, position.Y, size.X,
				size.Y, 0, 0, 1.0, 1.0);

			if (count.IsValid()) {
				std::stringstream ss;
				ss << *count;
				hud->DrawText(FString(UTF8_TO_TCHAR(ss.str().c_str())),
					FLinearColor(1, 1, 1), position.X + 5, position.Y + 5, font);
				ss.str(""); ss.clear();
			}
		} else {
			hud->DrawRect(FLinearColor(0.5, 0.5, 0.5), position.X,
				position.Y, size.X, size.Y);
		}
	}
		
	bool QuickbarElement::onMouseOver(const Point2D & position) { return true; }
	bool QuickbarElement::onMouseLeave(const Point2D & position) { return true; }
	bool QuickbarElement::onMouseDown(const MouseEvent & evt) { return true; }
	bool QuickbarElement::onMouseUp(const MouseEvent & evt) { return true; }
}

#include <Daedalus.h>
#include <Actors/GUI/DashboardElements.h>

#include <Actors/GUI/PlayerHUD.h>

namespace gui {
	using namespace utils;
	using namespace fauna;

	/********************************************************************************
	 * QuickuseBarElement
	 ********************************************************************************/

	QuickuseBarElement::QuickuseBarElement(const Point2D & origin, const Point2D & size) :
		HUDElement(origin, size), cachedQuickbarSize(0)
	{}
	
	QuickuseBarElement::QuickuseBarElement() : QuickuseBarElement({ 0, 0 }, { 0, 0 }) {}

	void QuickuseBarElement::tick() {
		// Update size.
		if (parent) {
			const auto & parentBounds = parent->getBounds();

			reposition(Point2D(0, parentBounds.size.Y));
		}
	}

	void QuickuseBarElement::drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) {}
		
	bool QuickuseBarElement::onMouseOver(const Point2D & position) { return true; }
	bool QuickuseBarElement::onMouseLeave(const Point2D & position) { return true; }
	bool QuickuseBarElement::onMouseDown(const MouseEvent & evt) { return true; }
	bool QuickuseBarElement::onMouseUp(const MouseEvent & evt) { return true; }

	void QuickuseBarElement::updateQuickbar(const QuickuseBarPtr & bar) {
		const double slotSize = 64, slotBorder = 4;
		const double barHeight = slotSize + slotBorder * 2;
		bool hasChanged = false;
		
		model = bar;

		// Check for quickbar size changes.
		Uint32 newSize = 0;
		if (model) {
			newSize = model->getMaxSize();

			for (; newSize > cachedQuickbarSize; cachedQuickbarSize++) {
				appendChild(HUDElementPtr(
					new QuickuseItemElement({ 0, 0 }, { slotSize, slotSize },
					(*model)[cachedQuickbarSize])));
				hasChanged = true;
			}
		}

		for (; cachedQuickbarSize > newSize; cachedQuickbarSize--) {
			removeChild(cachedQuickbarSize);
			hasChanged = true;
		}
		
		if (hasChanged) {
			const auto count = children.size();
			const auto calcOffset = [&](const Uint32 index) {
				return index * slotSize + (index + 1) * slotBorder;
			};
			
			const double y = -calcOffset(1) + slotBorder;
			for (Uint32 i = 0; i < count; i++) {
				const double x = calcOffset(i);
				children[i]->reposition({ x, y });
			}
			resize(Point2D(calcOffset(count), calcOffset(1)));
		}
	}



	/********************************************************************************
	 * QuickuseItemElement
	 ********************************************************************************/

	QuickuseItemElement::QuickuseItemElement(
		const Point2D & origin,
		const Point2D & size,
		const IQuickuseCPtr & model
	) : HUDElement(origin, size), model(model)
	{}

	void QuickuseItemElement::tick() {}

	void QuickuseItemElement::drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) {
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
		
	bool QuickuseItemElement::onMouseOver(const Point2D & position) { return true; }
	bool QuickuseItemElement::onMouseLeave(const Point2D & position) { return true; }
	bool QuickuseItemElement::onMouseDown(const MouseEvent & evt) { return true; }
	bool QuickuseItemElement::onMouseUp(const MouseEvent & evt) { return true; }



	/********************************************************************************
	 * QuickuseBarElement
	 ********************************************************************************/

	InventoryElement::InventoryElement(const Point2D & origin, const Point2D & size) :
		HUDElement(origin, size), cachedInventorySize(0), itemsPerRow(4)
	{}
	
	InventoryElement::InventoryElement() : InventoryElement({ 0, 0 }, { 0, 0 }) {}

	void InventoryElement::tick() {
		// Update size.
		if (parent) {
			const double slotSize = 64, slotBorder = 4;
			const double barHeight = slotSize + slotBorder * 2;
			const auto & parentBounds = parent->getBounds();
			reposition(Point2D(0, parentBounds.size.Y - barHeight));
		}
	}

	void InventoryElement::drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) {}
		
	bool InventoryElement::onMouseOver(const Point2D & position) { return true; }
	bool InventoryElement::onMouseLeave(const Point2D & position) { return true; }
	bool InventoryElement::onMouseDown(const MouseEvent & evt) { return true; }
	bool InventoryElement::onMouseUp(const MouseEvent & evt) { return true; }
	
	void InventoryElement::reflowElements() {
		const double slotSize = 64, slotBorder = 4;
		const auto calcOffset = [&](const Uint32 index) {
			return index * slotSize + (index + 1) * slotBorder;
		};

		// Inventory is arranged from left -> right, bottom -> up
		// TODO: allow inventory to have any arrangement direction
		const Uint32 rowCount = std::ceil(children.size() / itemsPerRow);

		for (Uint32 j = 0; j < rowCount; j++) {
			for (Uint32 i = 0; i < itemsPerRow; i++) {
				const double x = calcOffset(i);
				const double y = -calcOffset(j + 1);
				children[j * itemsPerRow + i]->reposition({ x, y });
			}
		}

		resize(Point2D(calcOffset(itemsPerRow), calcOffset(rowCount)));
	}

	void InventoryElement::updateData(const InventoryCPtr & inventory, const Uint32 rowItems) {
		const double slotSize = 64, slotBorder = 4;
		bool hasChanged = rowItems != itemsPerRow;
		
		model = inventory;
		itemsPerRow = rowItems;

		// Check for InventoryElement size changes.
		Uint32 newSize = 0;
		if (model) {
			newSize = model->GetMaxSize();

			for (; newSize > cachedInventorySize; cachedInventorySize++) {
				appendChild(HUDElementPtr(
					new QuickuseItemElement({ 0, 0 }, { slotSize, slotSize },
					(*model)[cachedInventorySize])));
				hasChanged = true;
			}
		}

		for (; cachedInventorySize > newSize; cachedInventorySize--) {
			removeChild(cachedInventorySize);
			hasChanged = true;
		}

		if (hasChanged)
			reflowElements();
	}
}

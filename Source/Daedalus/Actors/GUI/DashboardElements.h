#pragma once

#include <Actors/GUI/HUDElement.h>
#include <Models/Shortcut.h>
#include <Models/Fauna/CharacterData.h>

namespace gui {
	class QuickuseBarElement : public HUDElement {
	protected:
		fauna::QuickuseBarPtr model;
		Uint32 cachedQuickbarSize;



		virtual void tick();
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache);
		
		virtual bool onMouseOver(const utils::Point2D & position);
		virtual bool onMouseLeave(const utils::Point2D & position);
		virtual bool onMouseDown(const MouseEvent & evt);
		virtual bool onMouseUp(const MouseEvent & evt);

	public:
		QuickuseBarElement(const utils::Point2D & origin, const utils::Point2D & size);
		QuickuseBarElement();
		void updateQuickbar(const fauna::QuickuseBarPtr & bar);
	};
	
	using QuickbarPtr = std::shared_ptr<QuickuseBarElement>;



	class QuickuseItemElement : public HUDElement {
	protected:
		IQuickuseCPtr model;



		virtual void tick();
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache);
		
		virtual bool onMouseOver(const utils::Point2D & position);
		virtual bool onMouseLeave(const utils::Point2D & position);
		virtual bool onMouseDown(const MouseEvent & evt);
		virtual bool onMouseUp(const MouseEvent & evt);

	public:
		QuickuseItemElement(
			const utils::Point2D & origin, const utils::Point2D & size,
			const IQuickuseCPtr & model);
	};
	
	using QuickuseItemElementPtr = std::shared_ptr<QuickuseItemElement>;



	class InventoryElement : public HUDElement {
	protected:
		fauna::InventoryCPtr model;
		Uint32 cachedInventorySize;
		Uint32 itemsPerRow;



		virtual void tick();
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache);
		
		virtual bool onMouseOver(const utils::Point2D & position);
		virtual bool onMouseLeave(const utils::Point2D & position);
		virtual bool onMouseDown(const MouseEvent & evt);
		virtual bool onMouseUp(const MouseEvent & evt);

		void reflowElements();

	public:
		InventoryElement(const utils::Point2D & origin, const utils::Point2D & size);
		InventoryElement();
		void updateData(
			const fauna::InventoryCPtr & inventory, const Uint32 rowItems);
	};
	
	using InventoryElementPtr = std::shared_ptr<InventoryElement>;
}
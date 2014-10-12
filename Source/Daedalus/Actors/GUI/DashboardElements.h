#pragma once

#include <Actors/GUI/HUDElement.h>
#include <Models/Shortcut.h>
#include <Models/Fauna/CharacterData.h>

namespace gui {
	class QuickuseBarElement : public HUDElement {
	public:
		using QuickuseBarElementPtr = std::shared_ptr<QuickuseBarElement>;

	private:
		CursorElementPtr cursorRef;

	protected:
		fauna::QuickuseBarPtr model;
		Uint32 cachedQuickbarSize;



		void copyProperties(const QuickuseBarElementPtr & element) const;

		virtual QuickuseBarElement * createNew() const;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;
		
		virtual bool onMouseOver(const utils::Point2D & position) override;
		virtual bool onMouseLeave(const utils::Point2D & position) override;
		virtual bool onMouseDown(const MouseEvent & evt) override;
		virtual bool onMouseUp(const MouseEvent & evt, const bool isInside) override;

	public:
		QuickuseBarElement(
			const utils::Point2D & origin, const utils::Point2D & size,
			const CursorElementPtr & cursorRef);
		QuickuseBarElement(const CursorElementPtr & cursorRef);
		
		QuickuseBarElementPtr clone() const;

		void updateQuickbar(const fauna::QuickuseBarPtr & bar);
	};
	
	using QuickuseBarElementPtr = QuickuseBarElement::QuickuseBarElementPtr;



	class QuickuseItemElement : public DraggableElement {
	public:
		using QuickuseItemElementPtr = std::shared_ptr<QuickuseItemElement>;

	protected:
		IQuickuseCPtr model;



		virtual QuickuseItemElement * createNew() const;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;
		
		virtual bool onMouseOver(const utils::Point2D & position) override;
		virtual bool onMouseLeave(const utils::Point2D & position) override;
		virtual bool onMouseDown(const MouseEvent & evt) override;
		virtual bool onMouseUp(const MouseEvent & evt, const bool isInside) override;

	public:
		QuickuseItemElement(
			const utils::Point2D & origin, const utils::Point2D & size,
			const IQuickuseCPtr & model, const CursorElementPtr & cursorRef);
		
		QuickuseItemElementPtr clone() const;
	};
	
	using QuickuseItemElementPtr = QuickuseItemElement::QuickuseItemElementPtr;



	class InventoryElement : public HUDElement {
	public:
		using InventoryElementPtr = std::shared_ptr<InventoryElement>;

	private:
		CursorElementPtr cursorRef;

	protected:
		fauna::InventoryCPtr model;
		Uint32 cachedInventorySize;
		Uint32 itemsPerRow;



		void copyProperties(const InventoryElementPtr & element) const;

		virtual InventoryElement * createNew() const;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;
		
		virtual bool onMouseOver(const utils::Point2D & position) override;
		virtual bool onMouseLeave(const utils::Point2D & position) override;
		virtual bool onMouseDown(const MouseEvent & evt) override;
		virtual bool onMouseUp(const MouseEvent & evt, const bool isInside) override;

		void reflowElements();

	public:
		InventoryElement(
			const utils::Point2D & origin, const utils::Point2D & size,
			const CursorElementPtr & cursorRef);
		InventoryElement(const CursorElementPtr & cursorRef);
		
		InventoryElementPtr clone() const;

		void updateData(
			const fauna::InventoryCPtr & inventory, const Uint32 rowItems);
	};
	
	using InventoryElementPtr = InventoryElement::InventoryElementPtr;
}

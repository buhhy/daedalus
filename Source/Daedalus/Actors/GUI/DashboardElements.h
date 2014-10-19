#pragma once

#include <Actors/GUI/Draggable.h>
#include <Actors/GUI/Droppable.h>
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



	class QuickuseItemElement : public IDroppable<DraggableElement> {
	public:
		using QuickuseItemElementPtr = std::shared_ptr<QuickuseItemElement>;

	protected:
		fauna::QuickuseBarPtr quickuseBarRef;
		Uint32 elementIndex;



		virtual QuickuseItemElement * createNew() const;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;
		
		virtual bool onMouseOver(const utils::Point2D & position) override;
		virtual bool onMouseLeave(const utils::Point2D & position) override;
		virtual bool onMouseDown(const MouseEvent & evt) override;
		virtual bool onMouseUp(const MouseEvent & evt, const bool isInside) override;
		
		virtual bool onDrop(
			const HUDElementPtr & draggable, const utils::Point2D & position) override;

	public:
		QuickuseItemElement(
			const utils::Point2D & origin, const utils::Point2D & size,
			const fauna::QuickuseBarPtr quickuseBar, const Uint32 index,
			const CursorElementPtr & cursorRef);
		
		QuickuseItemElementPtr clone() const;
		Uint32 getElementIndex() const;
		IQuickusePtr getElementModel();
	};
	
	using QuickuseItemElementPtr = QuickuseItemElement::QuickuseItemElementPtr;



	class InventoryElement : public HUDElement {
	public:
		using InventoryElementPtr = std::shared_ptr<InventoryElement>;

	private:
		CursorElementPtr cursorRef;

	protected:
		fauna::InventoryPtr model;
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
			const fauna::InventoryPtr & inventory, const Uint32 rowItems);
	};
	
	using InventoryElementPtr = InventoryElement::InventoryElementPtr;



	class InventoryItemElement : public DraggableElement {
	public:
		using InventoryItemElementPtr = std::shared_ptr<InventoryItemElement>;

	protected:
		fauna::InventorySlotPtr model;



		virtual InventoryItemElement * createNew() const;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;
		
		virtual bool onMouseOver(const utils::Point2D & position) override;
		virtual bool onMouseLeave(const utils::Point2D & position) override;
		virtual bool onMouseDown(const MouseEvent & evt) override;
		virtual bool onMouseUp(const MouseEvent & evt, const bool isInside) override;

	public:
		InventoryItemElement(
			const utils::Point2D & origin, const utils::Point2D & size,
			const fauna::InventorySlotPtr & model, const CursorElementPtr & cursorRef);
		
		InventoryItemElementPtr clone() const;

		fauna::InventorySlotPtr getModel();
	};
	
	using InventoryItemElementPtr = InventoryItemElement::InventoryItemElementPtr;
}

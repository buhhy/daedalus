#pragma once

#include <Actors/GUI/HUDElement.h>

namespace gui {
	// Forward declarations.
	template <typename T>
	class IDraggable;
	template <typename T>
	class IDroppable;
	class CursorElement;
	using CursorElementPtr = std::shared_ptr<CursorElement>;
	using DraggableElementPtr = std::shared_ptr<IDraggable<HUDElement>>;
	using DraggableElementWPtr = std::weak_ptr<IDraggable<HUDElement>>;
	using DroppableElementPtr = std::shared_ptr<IDroppable<HUDElement>>;
	

	
	/**
	 * Represents a clone of an object that is currently being dragged by the cursor.
	 */
	class DragHolderElement : public HUDElement {
	public:
		using DragHolderElementPtr = std::shared_ptr<DragHolderElement>;
		using DragHolderElementWPtr = std::weak_ptr<DragHolderElement>;

	private:
		Uint32 originalChildIndex;           // Index of the element in its parent's child list.
		utils::Point2D originalPosition;
		HUDElementWPtr originalParentRef;



		void initializeElement(const utils::Point2D & cursorPosition);

	protected:
		// Defines the offset of this element to the cursor element.
		utils::Point2D offsetPosition;
		DraggableElementPtr placeholderRef;
		CursorElementPtr cursorRef;



		void revert();
		virtual DragHolderElement * createNew() const override;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache);

	public:
		DragHolderElement(const DraggableElementPtr & placeholder, const CursorElementPtr & cursor);
		DragHolderElement(
			const DraggableElementPtr & placeholder, const CursorElementPtr & cursor,
			const utils::Point2D & elementOffsetOverride);
		
		virtual void onAttach(const HUDElementPtr & newParent) override;

		/**
		 * @return True if the dragged element has been reverted, false otherwise.
		 */
		bool onDragAccept(const utils::Point2D & cursorPosition);
		/**
		 * @return True if the dragged element has been reverted, false otherwise.
		 */
		bool onDragReject(const utils::Point2D & cursorPosition);
		DraggableElementPtr getContainedElement();
		void onCursorReposition(const utils::Point2D & cursorPosition);
	};

	using DragHolderElementPtr = DragHolderElement::DragHolderElementPtr;



	/**
	 * Cursor element that also contains any dragged elements. Children elements are assumed
	 * to be dragged elements.
	 */
	class CursorElement : public HUDElement {
	public:
		using CursorElementPtr = std::shared_ptr<CursorElement>;
		using DraggableElementList = std::unordered_set<DragHolderElementPtr>;

	protected:
		DraggableElementList draggingElements;

		Uint8 mouseButtonsActive;
		utils::Point2D currentCursorPosition;
		utils::Point2D previousCursorPosition;
		MouseEvent::CursorType currentCursorType;
		MouseEvent::CursorType previousCursorType;



		virtual CursorElement * createNew() const;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;
		
		virtual void onMouseMove(const utils::Point2D & position) override;
		virtual bool onMouseDown(const MouseEvent & evt) override;
		virtual bool onMouseUp(const MouseEvent & evt, const bool isInside) override;

	public:
		CursorElement();

		CursorElementPtr clone() const;

		void addDragElement(const DragHolderElementPtr & element);
		void removeDragElement(const DragHolderElementPtr & element);
		void clearDragElements();
		const DraggableElementList & getDraggingElements() const;

		void setCursorPosition(const utils::Point2D & pos);
		const utils::Point2D & getCursorPosition() const;
	};

	using CursorElementPtr = CursorElement::CursorElementPtr;
}

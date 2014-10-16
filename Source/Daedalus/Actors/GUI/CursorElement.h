#pragma once

#include <Actors/GUI/HUDElement.h>
namespace gui {
	// Forward declarations.
	template <typename T>
	class IDraggable;
	class CursorElement;
	using CursorElementPtr = std::shared_ptr<CursorElement>;
	using DraggableElementPtr = std::shared_ptr<IDraggable<HUDElement>>;
	using DraggableElementWPtr = std::weak_ptr<IDraggable<HUDElement>>;
	

	
	/**
	 * Represents a clone of an object that is currently being dragged by the cursor.
	 */
	class DragHolderElement : public HUDElement {
	public:
		using DragHolderElementPtr = std::shared_ptr<DragHolderElement>;
		using DragHolderElementWPtr = std::weak_ptr<DragHolderElement>;

	protected:
		DraggableElementWPtr creatorRef;
		HUDElementPtr placeholderRef;
		CursorElementPtr cursorRef;



		virtual DragHolderElement * createNew() const override;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache);

	public:
		/**
		 * @param creator The original creator source of this drag holder element, the holder
		 *                element inherits its starting position from this provided element,
		 *                if this element pointer is expired, then the position is determined
		 *                from the mouse cursor.
		 */
		DragHolderElement(
			DraggableElementWPtr creator,
			const HUDElementPtr & placeholder,
			const CursorElementPtr & cursor);
		
		virtual void onAttach(const HUDElementPtr & newParent) override;
		HUDElementPtr getContainedElement();
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

		void setCursorPosition(const utils::Point2D & pos);

	public:
		CursorElement();

		CursorElementPtr clone() const;

		void startDragElement(const DragHolderElementPtr & element);
		void stopDragElement(const DragHolderElementPtr & element);
		const DraggableElementList & getDraggingElements() const;
	};

	using CursorElementPtr = CursorElement::CursorElementPtr;
}

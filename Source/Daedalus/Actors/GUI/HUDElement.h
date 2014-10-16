#pragma once

#include <Engine.h>
#include <Controllers/ResourceCache.h>
#include <Utilities/Algebra/Algebra2D.h>

#include <memory>
#include <vector>
#include <unordered_set>

class APlayerHUD;

namespace gui {
	struct MouseEvent {
		enum CursorType {
			C_Pointer,
			C_Hover,
			C_Active
		};

		static const Uint8 BUTTON_PRESS_LEFT = (Uint8) 0x1;
		static const Uint8 BUTTON_PRESS_MIDDLE = (Uint8) 0x2;
		static const Uint8 BUTTON_PRESS_RIGHT = (Uint8) 0x4;

		utils::Point2D position;
		Uint8 whichButtons;

		MouseEvent(
			const utils::Point2D & pos,
			const Uint8 which
		) : position(pos), whichButtons(which)
		{}
	};

	/**
	 * An element to be displayed on the HUD that contains convenience methods for hitbox
	 * detection and event handling. Based on the HTML DOM tree.
	 */
	class HUDElement : public std::enable_shared_from_this<HUDElement> {
	public:
		using HUDElementPtr = std::shared_ptr<HUDElement>;
		using HUDElementCPtr = std::shared_ptr<const HUDElement>;
		using HUDElementWPtr = std::weak_ptr<HUDElement>;
		using HUDElementList = std::vector<HUDElementPtr>;

	private:
		bool bIsMouseInside;

	protected:
		bool bIsHidden;

		utils::Box2D bounds;
		HUDElementList children;
		HUDElementWPtr parent;



		bool isMouseInside() const;

		/**
		 * This method is used in the clone method to copy data, whereas the clone method
		 * creates the new object pointer. We can't use covariance in parameters, so we
		 * won't be using inheritance here, each child will call its parent's overloaded
		 * copyProperties function.
		 */
		void copyProperties(const HUDElementPtr & element) const;
		/**
		 * This is used to simulate covariant shared pointer returns. When extending this
		 * base class, be sure to override both the createNew and the clone methods.
		 */
		virtual HUDElement * createNew() const = 0;

		virtual void tick() = 0;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) = 0;
		
		virtual void onMouseMove(const utils::Point2D & position);
		virtual bool onMouseOver(const utils::Point2D & position);
		virtual bool onMouseLeave(const utils::Point2D & position);
		virtual bool onMouseDown(const MouseEvent & evt);
		virtual bool onMouseUp(const MouseEvent & evt, const bool isInside);

		virtual void preMouseDown(const MouseEvent & evt);
		virtual void preMouseUp(const MouseEvent & evt, const bool isInside);

		virtual void onAttach(const HUDElementPtr & newParent);
		virtual void onDetach(const HUDElementPtr & oldParent);

	public:
		HUDElement(const utils::Point2D & origin, const utils::Point2D & size);
		HUDElement();

		HUDElementPtr clone() const;
		

		// Element tree traversal

		virtual bool checkMouseMove(const utils::Point2D & position);
		virtual bool checkMouseDown(const MouseEvent & evt);
		virtual bool checkMouseUp(const MouseEvent & evt);

		void drawElementTree(APlayerHUD * hud, const ResourceCacheCPtr & rcache);
		void runLogic(const float delta);

		bool hitTest(const utils::Point2D & pointerPos);


		// Children management

		/**
		 * Attaches this element as a child to the target element.
		 */
		void attachTo(const HUDElementPtr & parent);
		/**
		 * Removes this element from any parent element, if it exists.
		 */
		void detach();
		/**
		 * Detaches the child element from any pre-existing parents, then attach the
		 * child element to this element.
		 */
		void appendChild(const HUDElementPtr & child);
		void insertChild(const HUDElementPtr & child, const Uint32 index);
		/**
		 * Removes all instances of the provided child pointer, if they exist.
		 * @return true if any elements have been removed
		 */
		bool removeChild(const HUDElementPtr & possibleChild);
		HUDElementPtr removeChild(const Uint32 index);
		HUDElementPtr getChild(const Uint32 index);
		HUDElementCPtr getChild(const Uint32 index) const;
		HUDElementWPtr getParent();
		utils::Option<Uint32> findChildIndex(const HUDElementPtr & possibleChild) const;
		Uint32 childCount() const;


		// Position management
		
		void resize(const utils::Point2D & newSize);
		void reposition(const utils::Point2D & newPos);
		const utils::Box2D & getBounds() const;
		/**
		 * Retrieves the absolute position on the screen, after applying parent transforms
		 * in a recursive manner.
		 */
		utils::Point2D getAbsolutePosition() const;
		/**
		 * Retrieves the absolute bounds on the screen, after applying parent transforms
		 * in a recursive manner. This is not guaranteed to be needed until we begin
		 * applying transforms beyond translation.
		 */
		utils::Box2D getAbsoluteBounds() const;


		// Property management

		void setHidden(const bool isHidden);
		bool isHidden() const;
	};

	using HUDElementPtr = HUDElement::HUDElementPtr;
	using HUDElementCPtr = HUDElement::HUDElementCPtr;
	using HUDElementWPtr = HUDElement::HUDElementWPtr;


	
	class DivElement : public HUDElement {
	public:
		using DivElementPtr = std::shared_ptr<DivElement>;

	protected:
		virtual DivElement * createNew() const;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;

	public:
		DivElement(const utils::Point2D & origin, const utils::Point2D & size);
		DivElement();

		DivElementPtr clone() const;
	};
	
	using DivElementPtr = DivElement::DivElementPtr;



	// Forward declarations.
	template <typename T>
	class IDraggable;
	using DraggableElementPtr = std::shared_ptr<IDraggable<HUDElement>>;
	class DragHolderElement;
	using DragHolderElementPtr = std::shared_ptr<DragHolderElement>;

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



	/**
	 * Mixin for dragging functionality.
	 */
	template <typename T>
	class IDraggable : public T {
	public:
		using IDraggablePtr = std::shared_ptr<IDraggable<T>>;
		using IDraggableCPtr = std::shared_ptr<const IDraggable<T>>;
		using IDraggableWPtr = std::weak_ptr<IDraggable<T>>;

		struct DraggableParameters {
			enum DragType {
				DT_NoAction,
				DT_DragMove,
				DT_DragCopy
			};

			DragType dragType;
		};

	private:
		bool bIsDragging;
		Uint32 originalChildIndex;           // Index of the element in its parent's child list.
		utils::Point2D originalPosition;
		HUDElementWPtr originalParent;

	protected:
		bool bIsDragEnabled;
		DraggableParameters parameters;
		CursorElementPtr cursorRef;
		IDraggableWPtr placeholderRef;



		IDraggable(
			const DraggableParameters & params,
			const CursorElementPtr & cursor
		) : parameters(params), cursorRef(cursor), bIsDragging(false),
			originalChildIndex(0), originalPosition(0), bIsDragEnabled(true)
		{}
		
		void copyProperties(const HUDElementPtr & element) const {
			T::copyProperties(element);
		}

		virtual IDraggable * createNew() const = 0;

		bool isDragging() const {
			return bIsDragging;
		}

		/**
		 * @return true if the drag should start, false if it shouldn't start
		 */
		virtual bool onDragStart(const utils::Point2D & position) { return true; }
		virtual bool onDragEnd(const utils::Point2D & position) { return true; }

		/**
		 * Creates a placeholder object that contains a copy of the current element.
		 */
		virtual DragHolderElementPtr createPlaceholderCopy() {
			auto newEl = clone();
			newEl->setDragEnabled(false);
			return DragHolderElementPtr(new DragHolderElement(sharedPtr(), newEl, cursorRef));
		}
		
		virtual void preMouseDown(const MouseEvent & evt) override {
			if (bIsDragEnabled && !bIsDragging &&
				evt.whichButtons & MouseEvent::BUTTON_PRESS_LEFT)
			{
				bIsDragging = true;
				if (onDragStart(evt.position)) {
					if (parameters.dragType == DraggableParameters::DT_DragMove ||
						parameters.dragType == DraggableParameters::DT_DragCopy)
					{
						DragHolderElementPtr dragHolder = nullptr;
						if (parameters.dragType == DraggableParameters::DT_DragCopy) {
							// Create a new copy of the current element being dragged.
							dragHolder = createPlaceholderCopy();
						} else {
							dragHolder = DragHolderElementPtr(new DragHolderElement(
								sharedPtr(), sharedPtr(), cursorRef));
						}

						if (dragHolder) {
							// Store original position & parent.
							originalPosition = T::getBounds().origin;
							originalParent = T::getParent();

							const auto lockedParent = originalParent.lock();
							if (lockedParent) {
								const auto index = lockedParent->findChildIndex(sharedPtr());
								if (index.IsValid())
									originalChildIndex = *index;
								else
									originalParent.reset();
							}

							// Reposition dragged element to be absolute to world.
							cursorRef->startDragElement(dragHolder);
						}
					}
				}
			}
		}

		virtual void preMouseUp(const MouseEvent & evt, const bool isInside) override {
			//if (bIsDragging && evt.whichButtons & MouseEvent::BUTTON_PRESS_LEFT) {
			//	bIsDragging = false;
			//	if (onDragEnd(evt.position)) {
			//		if (parameters.dragType == DraggableParameters::DT_DragMove) {
			//			cursorRef->stopDragElement(sharedPtr());
			//			if (!originalParent.expired()) {
			//				T::reposition(originalPosition);
			//				originalParent.lock()->insertChild(sharedPtr(), originalChildIndex);
			//			}
			//		} else if (parameters.dragType == DraggableParameters::DT_DragCopy) {
			//			if (!placeholderRef.expired()) {
			//				placeholderRef.lock()->detach();
			//				placeholderRef.reset();
			//			}
			//		}
			//	}
			//}
		}

	public:
		DraggableElementPtr clone() const {
			auto created = DraggableElementPtr(createNew());
			copyProperties(created);
			return created;
		}
		
		IDraggablePtr sharedPtr() {
			return std::static_pointer_cast<IDraggable<T>>(T::shared_from_this());
		}

		IDraggableCPtr sharedPtr() const {
			return std::static_pointer_cast<IDraggable<T>>(T::shared_from_this());
		}

		bool isDragEnabled() const {
			return bIsDragEnabled;
		}

		void setDragEnabled(const bool enabled) {
			bIsDragEnabled = enabled;
		}
	};

	using DraggableElement = IDraggable<HUDElement>;
	using DraggableElementPtr = DraggableElement::IDraggablePtr;
	

	
	/**
	 * Represents a clone of an object that is currently dragging.
	 */
	class DragHolderElement : public HUDElement {
	public:
		using DragHolderElementPtr = std::shared_ptr<DragHolderElement>;
		using DragHolderElementWPtr = std::weak_ptr<DragHolderElement>;

	protected:
		DraggableElement::IDraggableWPtr creatorRef;
		HUDElementPtr placeholderRef;
		CursorElementPtr cursorRef;



		virtual DragHolderElement * createNew() const override {
			return new DragHolderElement(creatorRef, placeholderRef, cursorRef);
		}

		virtual void tick() override {}
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override {}

	public:
		/**
		 * @param creator The original creator source of this drag holder element, the holder
		 *                element inherits its starting position from this provided element,
		 *                if this element pointer is expired, then the position is determined
		 *                from the mouse cursor.
		 */
		DragHolderElement(
			DraggableElement::IDraggableWPtr creator,
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
		
		virtual void onAttach(const HUDElementPtr & newParent) override {
			placeholderRef->attachTo(shared_from_this());
		}

		HUDElementPtr getContainedElement() {
			return placeholderRef;
		}
	};

	using DragHolderElementPtr = DragHolderElement::DragHolderElementPtr;



	template <typename T>
	class IDroppable : public T {
	public:
		using DroppableElement = IDroppable<HUDElement>;
		using DroppableElementPtr = std::shared_ptr<DroppableElement>;

		struct DroppableParameters {
		};

	protected:
		bool bIsDropEnabled;
		DroppableParameters parameters;
		CursorElementPtr cursorRef;



		IDroppable(
			const DroppableParameters & params,
			const CursorElementPtr & cursor
		) : parameters(params), cursorRef(cursor), bIsDropEnabled(true)
		{}

		void copyProperties(const HUDElementPtr & element) const {
			T::copyProperties(element);
		}

		virtual IDroppable * createNew() const = 0;

		/**
		 * @return true if the element dropped should be consumed, false otherwise
		 */
		virtual bool onDrop(
			const HUDElementPtr & draggable,
			const utils::Point2D & position
		) {
			return true;
		}
		
		virtual void preMouseDown(const MouseEvent & evt) override {}

		virtual void preMouseUp(const MouseEvent & evt, const bool isInside) override {
			if (bIsDropEnabled && evt.whichButtons & MouseEvent::BUTTON_PRESS_LEFT) {
				const auto & elements = cursorRef->getDraggingElements();
				for (const auto element : elements) {
					if (onDrop(element->getContainedElement(), evt.position)) {
						// Stop dragging the element.
					}
				}
			}
		}

	public:
		DroppableElementPtr clone() const {
			auto created = DroppableElementPtr(createNew());
			copyProperties(created);
			return created;
		}

		DroppableElementPtr sharedPtr() {
			return std::static_pointer_cast<IDroppable<T>>(T::shared_from_this());
		}

		bool isDropEnabled() const {
			return bIsDropEnabled;
		}

		void setDropEnabled(const bool enabled) {
			bIsDropEnabled = enabled;
		}
	};

	using DroppableElement = IDroppable<HUDElement>::DroppableElementPtr;
}
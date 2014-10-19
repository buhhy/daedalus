#pragma once

#include <Actors/GUI/HUDElement.h>
#include <Actors/GUI/CursorElement.h>

namespace gui {
	// Forward declarations
	template <typename T>
	class IDroppable;
	template <typename T>
	using IDroppablePtr = std::shared_ptr<IDroppable<T>>;
	using DroppableElementPtr = IDroppablePtr<HUDElement>;

	struct DraggableParameters {
		enum DragType {
			DT_NoAction,
			DT_DragMove,
			DT_DragCopy
		};

		enum RevertAction {
			RA_RevertAll,
			RA_RevertInvalid,
			RA_RevertNone
		};

		DragType dragType;
		RevertAction revertAction;

		DraggableParameters(
			const DragType dt = DT_DragMove,
			const RevertAction ra = RA_RevertAll
		) : dragType(dt), revertAction(ra)
		{}
	};

	/**
	 * Mixin for dragging functionality.
	 */
	template <typename T>
	class IDraggable : public T {
	public:
		using IDraggablePtr = std::shared_ptr<IDraggable<T>>;
		using IDraggableCPtr = std::shared_ptr<const IDraggable<T>>;
		using IDraggableWPtr = std::weak_ptr<IDraggable<T>>;

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



		template <typename ... A>
		IDraggable(
			const DraggableParameters & params,
			const CursorElementPtr & cursor,
			const A & ... passthru
		) : T(passthru...), parameters(params), cursorRef(cursor), bIsDragging(false),
			originalChildIndex(0), originalPosition(0), bIsDragEnabled(true)
		{}

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
		//virtual bool onDragEnd(const utils::Point2D & position) { return true; }

		/**
		 * Creates a placeholder object that contains a copy of the current element.
		 */
		virtual DragHolderElementPtr createPlaceholderCopy() {
			auto newEl = clone();
			newEl->setDragEnabled(false);
			return DragHolderElementPtr(
				new DragHolderElement(
					newEl, cursorRef, getAbsolutePosition() - cursorRef->getCursorPosition()));
		}
		
		virtual void preMouseDown(const MouseEvent & evt) override {
			T::preMouseDown(evt);
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
							dragHolder = DragHolderElementPtr(
								new DragHolderElement(shared_from_this(), cursorRef));
						}

						if (dragHolder) {
							// Store original position & parent.
							originalPosition = T::getBounds().origin;
							originalParent = T::getParent();

							const auto lockedParent = originalParent.lock();
							if (lockedParent) {
								const auto index = lockedParent->findChildIndex(shared_from_this());
								if (index.IsValid())
									originalChildIndex = *index;
								else
									originalParent.reset();
							}

							// Reposition dragged element to be absolute to world.
							cursorRef->addDragElement(dragHolder);
						}
					}
				}
			}
		}

		virtual void preMouseUp(const MouseEvent & evt, const bool isInside) override {
			T::preMouseUp(evt, isInside);
		}

	public:
		DraggableElementPtr clone() const {
			auto created = DraggableElementPtr(createNew());
			copyProperties(created);
			return created;
		}
		
		IDraggablePtr shared_from_this() {
			return std::static_pointer_cast<IDraggable<T>>(T::shared_from_this());
		}

		IDraggableCPtr shared_from_this() const {
			return std::static_pointer_cast<IDraggable<T>>(T::shared_from_this());
		}

		bool isDragEnabled() const {
			return bIsDragEnabled;
		}

		void setDragEnabled(const bool enabled) {
			bIsDragEnabled = enabled;
		}

		virtual void onDragReject(const utils::Point2D & cursorPosition) {}
		virtual void onDragAccept(const utils::Point2D & cursorPosition) {}

		const DraggableParameters & getParameters() const {
			return parameters;
		}
	};

	using DraggableElement = IDraggable<HUDElement>;
	using DraggableElementPtr = DraggableElement::IDraggablePtr;
	using DraggableElementWPtr = DraggableElement::IDraggableWPtr;
}
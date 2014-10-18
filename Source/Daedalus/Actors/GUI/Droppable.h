#pragma once

#include <Actors/GUI/HUDElement.h>
#include <Actors/GUI/CursorElement.h>

namespace gui {
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
						// Element has stopped dragging.
						cursorRef->stopDragElement(element);
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

	using DroppableElement = IDroppable<HUDElement>;
	using DroppableElementPtr = DroppableElement::DroppableElementPtr;
}
#pragma once

#include <Actors/GUI/HUDElement.h>
#include <Actors/GUI/CursorElement.h>

namespace gui {
template <typename T>
	class IDroppable : public T {
	public:
		using IDroppablePtr = std::shared_ptr<IDroppable<T>>;
		using DroppableElement = IDroppable<HUDElement>;
		using DroppableElementPtr = std::shared_ptr<DroppableElement>;

		struct DroppableParameters {
		};

	protected:
		bool bIsDropEnabled;
		DroppableParameters parameters;
		CursorElementPtr cursorRef;



		template <typename ... A>
		IDroppable(
			const DroppableParameters & params,
			const CursorElementPtr & cursor,
			const A & ... passthru
		) : T(passthru...), parameters(params), cursorRef(cursor), bIsDropEnabled(true)
		{}

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
		 * @return True if the element dropped is accepted, false otherwise.
		 */
		virtual bool onDrop(
			const HUDElementPtr & draggable,
			const utils::Point2D & position
		) {
			return true;
		}

		/**
		 * Default behaviour when an element is accepted is to attach it as a child.
		 */
		virtual void acceptElement(const HUDElementPtr & draggable) {
			draggable->reposition(
				T::convertToRelativePosition(draggable->getAbsolutePosition()));
			T::appendChild(draggable);
		}
		
		virtual void preMouseDown(const MouseEvent & evt) override {
			T::preMouseDown(evt);
		}

		virtual void preMouseUp(const MouseEvent & evt, const bool isInside) override {
			T::preMouseUp(evt, isInside);
			if (isInside && bIsDropEnabled && evt.whichButtons & MouseEvent::BUTTON_PRESS_LEFT) {
				const auto & elements = cursorRef->getDraggingElements();
				for (const auto element : elements) {
					bool reverted = true;
					if (onDrop(element->getContainedElement(), evt.position)) {
						// Element has been accepted.
						reverted = element->onDragAccept(evt.position);
					} else {
						// Element has been rejected.
						reverted = element->onDragReject(evt.position);
					}

					// If the element hasn't been reverted to its original state, then accept it
					// onto this element.
					if (!reverted)
						acceptElement(element);
				}
				cursorRef->clearDragElements();
			}
		}

	public:
		IDroppablePtr clone() const {
			auto created = DroppableElementPtr(createNew());
			copyProperties(created);
			return created;
		}

		IDroppablePtr shared_from_this() {
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
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
}

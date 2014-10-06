#pragma once

#include <Engine.h>
#include <Controllers/ResourceCache.h>
#include <Utilities/Algebra/Algebra2D.h>

#include <memory>
#include <vector>

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
		Uint8 activeButtons;

		MouseEvent(
			const utils::Point2D & pos,
			const Uint8 which
		) : position(pos), activeButtons(which)
		{}
	};

	/**
	 * An element to be displayed on the HUD that contains convenience methods for hitbox
	 * detection and event handling. Based on the HTML DOM tree.
	 */
	class HUDElement : public std::enable_shared_from_this<HUDElement> {
	public:
		using HUDElementList = std::vector<std::shared_ptr<HUDElement>>;

	private:
		bool bIsMouseInside;

	protected:
		bool bIsHidden;

		utils::Box2D bounds;
		HUDElementList children;
		std::shared_ptr<HUDElement> parent;



		bool isMouseInside() const;

		virtual void tick();
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache);
		
		virtual void onMouseMove(const utils::Point2D & position);
		virtual bool onMouseOver(const utils::Point2D & position);
		virtual bool onMouseLeave(const utils::Point2D & position);
		virtual bool onMouseDown(const MouseEvent & evt);
		virtual bool onMouseUp(const MouseEvent & evt);

		virtual void preMouseDown(const MouseEvent & evt);
		virtual void preMouseUp(const MouseEvent & evt);

	public:
		HUDElement(const utils::Point2D & origin, const utils::Point2D & size);
		HUDElement();
		

		// Element tree traversal

		virtual bool checkMouseMove(const utils::Point2D & position);
		virtual bool checkMouseDown(const MouseEvent & evt);
		virtual bool checkMouseUp(const MouseEvent & evt);

		void drawElementTree(APlayerHUD * hud, const ResourceCacheCPtr & rcache);
		void runLogic(const float delta);

		bool hitTest(const utils::Point2D & pointerPos);


		// Children management

		void attachTo(const std::shared_ptr<HUDElement> & parent);
		bool appendChild(const std::shared_ptr<HUDElement> & child);
		std::shared_ptr<HUDElement> removeChild(const Uint32 index);
		std::shared_ptr<HUDElement> getChild(const Uint32 index);
		std::shared_ptr<HUDElement> getParent();
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


		// Property management

		void setHidden(const bool isHidden);
		bool isHidden() const;
	};

	using HUDElementPtr = std::shared_ptr<HUDElement>;



	template <typename T>
	class IDraggable : T {
	private:
		bool bIsDragging;
		utils::Point2D & mouseDragLocation;

	protected:
		bool isDragging() const { return bIsDragging; }
	
		virtual bool onDragStart(const utils::Point2D & position) { return true; }
		virtual bool onDragEnd(const utils::Point2D & position) { return true; }
		
		virtual bool preMouseDown(const MouseEvent & evt) override {
			if (evt.activeButtons & MouseEvent::BUTTON_PRESS_LEFT && !bIsDragging) {
				bIsDragging = true;
				onDragStart(evt.position);
			}
		}

		virtual bool preMouseUp(const MouseEvent & evt) override {
			if (evt.activeButtons & MouseEvent::BUTTON_PRESS_LEFT && bIsDragging) {
				bIsDragging = false;
				onDragEnd(evt.position);
			}
		}

	public:
	};

	using DraggableElement = IDraggable<HUDElement>;



	template <typename T>
	class IDroppable : T {
	public:
		virtual bool checkMouseMove(const utils::Point2D & position);
		virtual bool checkMouseDown(const MouseEvent & evt);
		virtual bool checkMouseUp(const MouseEvent & evt);
	};

	using DroppableElement = IDroppable<HUDElement>;



	/**
	 * Cursor element that also contains any dragged elements.
	 */
	class CursorElement : public HUDElement {
	protected:
		Uint8 mouseButtonsActive;
		utils::Point2D cursorPosition;
		MouseEvent::CursorType currentCursorType;
		MouseEvent::CursorType previousCursorType;



		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;
		
		virtual void onMouseMove(const utils::Point2D & position) override;
		virtual bool onMouseDown(const MouseEvent & evt) override;
		virtual bool onMouseUp(const MouseEvent & evt) override;
	};

	using CursorElementPtr = std::shared_ptr<CursorElement>;
}
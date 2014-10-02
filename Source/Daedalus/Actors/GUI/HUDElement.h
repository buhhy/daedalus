#pragma once

#include <Engine.h>
#include <Controllers/ResourceCache.h>
#include <Models/Shortcut.h>
#include <Models/Fauna/CharacterData.h>
#include <Utilities/Algebra/Algebra2D.h>

#include <memory>
#include <vector>

namespace gui {
	struct MouseEvent {
		utils::Point2D position;
		Uint8 whichBtn;

		MouseEvent(
			const utils::Point2D & pos,
			const Uint8 which
		) : position(pos), whichBtn(which)
		{}
	};

	/**
	 * An element to be displayed on the HUD that contains convenience methods for hitbox
	 * detection and event handling.
	 */
	class HUDElement : public std::enable_shared_from_this<HUDElement> {
	public:
		using HUDElementList = std::vector<std::shared_ptr<HUDElement>>;

	private:
		bool bIsMouseInside;

	protected:
		utils::Box2D bounds;
		HUDElementList children;
		std::shared_ptr<HUDElement> parent;



		bool isMouseInside() const;

		virtual void tick();
		virtual void drawElement(AHUD * hud, const ResourceCacheCPtr & rcache);
		
		virtual bool onMouseOver(const utils::Point2D & position);
		virtual bool onMouseLeave(const utils::Point2D & position);
		virtual bool onMouseDown(const MouseEvent & evt);
		virtual bool onMouseUp(const MouseEvent & evt);

	public:
		HUDElement(const utils::Point2D & origin, const utils::Point2D & size);
		HUDElement();

		bool checkMouseMove(const utils::Point2D & position);
		bool checkMouseDown(const MouseEvent & evt);
		bool checkMouseUp(const MouseEvent & evt);

		void drawElementTree(AHUD * hud, const ResourceCacheCPtr & rcache);
		void runLogic(const float delta);

		bool hitTest(const utils::Point2D & pointerPos);

		void attachTo(const std::shared_ptr<HUDElement> & parent);
		bool appendChild(const std::shared_ptr<HUDElement> & child);
		std::shared_ptr<HUDElement> removeChild(const Uint32 index);
		std::shared_ptr<HUDElement> getChild(const Uint32 index);
		std::shared_ptr<HUDElement> getParent();
		Uint32 childCount() const;
		
		void resize(const utils::Point2D & newSize);
		void reposition(const utils::Point2D & newPos);
		const utils::Box2D & getBounds() const;
		/**
		 * Retrieves the absolute position on the screen, after applying parent transforms
		 * in a recursive manner.
		 */
		utils::Point2D getAbsolutePosition() const;
	};

	using HUDElementPtr = std::shared_ptr<HUDElement>;



	class Quickbar : public HUDElement {
	protected:
		fauna::ShortcutBarPtr model;
		Uint32 cachedQuickbarSize;



		virtual void tick();
		virtual void drawElement(AHUD * hud, const ResourceCacheCPtr & rcache);
		
		virtual bool onMouseOver(const utils::Point2D & position);
		virtual bool onMouseLeave(const utils::Point2D & position);
		virtual bool onMouseDown(const MouseEvent & evt);
		virtual bool onMouseUp(const MouseEvent & evt);

	public:
		Quickbar(const utils::Point2D & origin, const utils::Point2D & size);
		Quickbar();
		void updateQuickbar(const fauna::ShortcutBarPtr & bar);
	};
	
	using QuickbarPtr = std::shared_ptr<Quickbar>;



	class QuickbarElement : public HUDElement {
	protected:
		ShortcutPtr model;



		virtual void tick();
		virtual void drawElement(AHUD * hud, const ResourceCacheCPtr & rcache);
		
		virtual bool onMouseOver(const utils::Point2D & position);
		virtual bool onMouseLeave(const utils::Point2D & position);
		virtual bool onMouseDown(const MouseEvent & evt);
		virtual bool onMouseUp(const MouseEvent & evt);

	public:
		QuickbarElement(
			const utils::Point2D & origin, const utils::Point2D & size,
			const ShortcutPtr & model);
	};
	
	using QuickbarElementPtr = std::shared_ptr<QuickbarElement>;
}
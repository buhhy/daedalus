#pragma once

#include <Actors/GUI/CursorElement.h>
#include <Actors/GUI/Droppable.h>
#include <Actors/GUI/HUDElement.h>

namespace gui {
	/**
	 * Your basic multi-purpose container element.
	 */
	class DivElement : public HUDElement {
	public:
		using DivElementPtr = std::shared_ptr<DivElement>;

	protected:
		virtual DivElement * createNew() const override;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;

	public:
		DivElement(const utils::Point2D & origin, const utils::Point2D & size);
		DivElement();

		DivElementPtr clone() const;
	};
	
	using DivElementPtr = DivElement::DivElementPtr;



	/**
	 * The root element that contains the entire element tree. This element accepts
	 * any dropped element, and cancels the drag operation if it isn't consumed by an
	 * element in the DOM tree.
	 */
	class DocumentRootElement : public DroppableElement {
	public:
		using DocumentRootElementPtr = std::shared_ptr<DocumentRootElement>;

	protected:
		virtual DocumentRootElement * createNew() const override;
		virtual void tick() override;
		virtual void drawElement(APlayerHUD * hud, const ResourceCacheCPtr & rcache) override;
		virtual bool onDrop(
			const HUDElementPtr & draggable, const utils::Point2D & position) override;

	public:
		DocumentRootElement(const CursorElementPtr & cursor);

		DocumentRootElementPtr clone() const;
	};
	
	using DocumentRootElementPtr = DocumentRootElement::DocumentRootElementPtr;
}
#pragma once

// include dependencies
#include "units.hpp"
#include "event.hpp"
#include "buffer.hpp"
#include <functional>
#include <deque>
#include <memory>

namespace ui {
	/// Contains thickness for borders in 4 directions.
	struct Borders {
		int left   = 0; /// Thickness of left border.
		int right  = 0; /// Thickness of right border.
		int top    = 0; /// Thickness of top border.
		int bottom = 0; /// Thickness of bottom border.

		/// Reserves space for borders in a rectangle.
		/// @return New shrinked rectangle.
		sf::IntRect apply(sf::IntRect rect) const;
	};

	/// Base UI element object.
	class Element {
	public:
		/// Event handler function type.
		/// 
		/// @param self Updated element.
		/// @param evt Event object.
		/// 
		/// @return Whether the event has been absorbed.
		using EventHandler = std::function<bool(Element& self, const Event& evt)>;
		/// Update handler function type.
		/// 
		/// @param self Updated element.
		/// @param delta Time elapsed since last frame.
		using UpdateHandler = std::function<void(Element& self, float delta)>;

	private:
		/// Element's children.
		std::deque<std::unique_ptr<Element>> _elements;
		/// Element's parent.
		Element* _parent = nullptr;

		sf::IntRect _outerRect; /// Draw area from parent container.
		sf::IntRect _rect;      /// Draw area after padding.
		sf::IntRect _innerRect; /// Draw area after padding & margin.

		/// Whether the element was being hovered over.
		bool _hover_old = false;
		/// Whether the element is being hovered over.
		bool _hover_now = false;
		/// Event handler list.
		std::deque<EventHandler> _handle_list;
		/// Post-event update handler list.
		std::deque<UpdateHandler> _update_list;

	public:
		DimRect bounds;           /// Element bounding box.
		Borders padding;          /// Element padding.
		Borders margin;           /// Element margin.
		bool active      = true;  /// Whether the element responds to events and is drawn.
		bool ignore      = false; /// Whether the element ignores events.
		bool transparent = false; /// Whether mouse hover events can pass through the element.

	protected:
		/// Draws the element.
		/// 
		/// @param target Render buffer.
		/// @param self Element drawing area.
		virtual void drawSelf(RenderBuffer& target, sf::IntRect self) const;

		/// Draws element children.
		/// 
		/// @param target Render buffer.
		virtual void drawChildren(RenderBuffer& target) const;

		/// Performs an operation before recalculation.
		virtual void recalcUpdate();
			
	public:
		/// Virtual destructor.
		virtual ~Element();

		/// Adds new element as a child.
		/// 
		/// Pointer to element must be owning.
		/// @param element New child element.
		void add(Element* element);
		/// Removes a child element if present.
		/// @param element Old child element.
		void remove(Element* element);
		/// Replaces old element by a new one.
		/// 
		/// If the old element is not found, the new one will be added with `add(Element*)`.
		/// 
		/// @param old Old element pointer.
		/// @param repl New element pointer.
		void replace(Element* old, Element* repl);

		/// Recalculates draw area for the element.
		/// @param parent Parent bounding box.
		void recalculate(sf::IntRect parent);
		/// Draws the element and its children.
		/// @param target Render buffer.
		void draw(RenderBuffer& target) const;
		/// Updates element and its children.
		/// @param delta Time elapsed since last frame.
		void update(float delta);

		/// Emits an event to the element.
		/// 
		/// @param evt Event object.
		/// 
		/// @return Whether the event has been absorbed.
		bool event(Event evt);
		/// Handles the event.
		/// 
		/// @param evt Event object.
		/// 
		/// @return Whether the event has been absorbed.
		bool handle(Event evt);
		/// Processes mouse hovering events.
		/// 
		/// @param pos Mouse position.
		/// 
		/// @return Whether the hover has been absorbed.
		bool hover(sf::Vector2i pos);

		/// Attaches an event handler.
		/// @param handler Event handler.
		void onEvent(const EventHandler& handler);
		/// Attaches an update handler.
		/// @param handler Update handler.
		void onUpdate(const UpdateHandler& handler);

		/// @return Element's children.
		const std::deque<std::unique_ptr<Element>>& children() const;
		/// @return Element's parent.
		Element* parent() const;

		/// @return Element position.
		DimVector& position();
		const DimVector& position() const;
		/// @return Element size.
		DimVector& size();
		const DimVector& size() const;

		/// @return Element bounding rectangle (available after recalculation).
		sf::IntRect rect() const;
	};
};
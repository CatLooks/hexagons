#pragma once

// include dependencies
#include <memory>
#include <deque>
#include <functional>
#include "units.hpp"
#include "events.hpp"

namespace ui {
	/// Contains thickness for borders in 4 directions.
	struct Borders {
		int left   = 0; /// Thickness of left border.
		int right  = 0; /// Thickness of right border.
		int top    = 0; /// Thickness of top border.
		int bottom = 0; /// Thickness of bottom border.

		/// Reserves space for borders in a rectangle.
		/// @return New shrinked rectangle.
		sf::IntRect apply(sf::IntRect rect) const {
			return {
				{ rect.position.x + left, rect.position.y + top },
				{ rect.size.x - left - right, rect.size.y - top - bottom }
			};
		};
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
		bool active      = true;  /// Whether the element responds to events and is drawn.
		bool ignore      = false; /// Whether the element ignores events.
		bool transparent = false; /// Whether mouse events can pass through the element.
		
		Borders padding; /// Element padding.
		Borders margin;  /// Element margin.

	protected:
		/// Draws the element.
		/// @param self Element drawing area.
		virtual void drawSelf(sf::IntRect self) const {};

		/// Draws element children.
		virtual void drawChildren() const {
			for (const auto& element : _elements)
				element->draw();
		};

		/// Performs an operation before recalculation.
		virtual void preRecalc() {};
			
	public:
		/// Virtual destructor.
		virtual ~Element() {};

		/// Adds new element as a child.
		/// 
		/// Pointer to element must be owning.
		/// @param element New child element.
		void add(Element* element) {
			_elements.push_back(std::unique_ptr<Element>(element));
			element->_parent = this;
		};
		/// Removes a child element if present.
		/// @param element Old child element.
		void remove(Element* element) {
			for (auto it = _elements.begin(); it != _elements.end(); it++) {
				if (it->get() == element) {
					_elements.erase(it);
					return;
				};
			};
		};
		/// Replaces old element by a new one.
		/// 
		/// If the old element is not found, the new one will be added with `add(Element*)`.
		/// 
		/// @param old Old element pointer.
		/// @param repl New element pointer.
		void replace(Element* old, Element* repl) {
			// try replacing old element
			for (auto it = _elements.begin(); it != _elements.end(); it++) {
				if (it->get() == old) {
					*it = std::unique_ptr<Element>(repl);
					repl->_parent = this;
					return;
				};
			};

			// add if old was not found
			add(repl);
		};

		/// Recalculates draw area for the element.
		/// @param parent Parent bounding box.
		void recalculate(sf::IntRect parent) {
			// ignore if not active
			if (!active) return;

			// update before recalculation
			preRecalc();

			// recalculate element draw areas
			_outerRect = bounds.get(parent);
			_rect = padding.apply(_outerRect);
			_innerRect = margin.apply(_rect);

			// recalculate children
			for (const auto& element : _elements)
				element->recalculate(_innerRect);
		};

		/// Draws the element and its children.
		void draw() const {
			// ignore if not active
			if (!active) return;

			// draw self & children
			drawSelf(_rect);
			drawChildren();
		};

		/// Updates element and its children.
		/// @param delta Time elapsed since last frame.
		void update(float delta) {
			// ignore if not active
			if (!active) return;

			// update element
			for (const auto& handler : _update_list)
				handler(*this, delta);

			// update children
			for (const auto& element : _elements)
				element->update(delta);
		};

		/// Emits an event to the element.
		/// 
		/// @param evt Event object.
		/// 
		/// @return Whether the event has been absorbed.
		bool event(Event evt) {
			// ignore if needed
			if (!active || ignore) return false;

			// send event to children
			for (const auto& element : _elements)
				if (element->event(evt))
					return true;

			// check if mouse is out of range for mouse events
			if (auto pos = evt.mouse())
				if (!_rect.contains(*pos))
					return false;

			// handle the event
			return handle(evt);
		};
		/// Handles the event.
		/// 
		/// @param evt Event object.
		/// 
		/// @return Whether the event has been absorbed.
		bool handle(Event evt) {
			bool absorb = false;
			for (const auto& handler : _handle_list)
				if (handler(*this, evt))
					absorb = true;
			return absorb;
		};

		/// Processes mouse hovering events.
		/// 
		/// @param pos Mouse position.
		/// 
		/// @return Whether the hover has been absorbed.
		bool hover(sf::Vector2i pos) {
			// store hover state
			_hover_old = _hover_now;
			_hover_now = false;

			// ignore if needed
			if (!active || ignore) return false;

			// update children
			for (const auto& element : _elements)
				if (element->hover(pos))
					return true;

			// update hover state
			_hover_now = _rect.contains(pos);

			// trigger mouse events
			if (_hover_now && !_hover_old) handle((Event)Event::MouseEnter { pos });
			if (!_hover_now && _hover_old) handle((Event)Event::MouseLeave { pos });
			return _hover_now && !transparent;
		};

		/// Attaches an event handler.
		/// @param handler Event handler.
		void onEvent(const EventHandler& handler) { _handle_list.push_back(handler); };
		/// Attaches an update handler.
		/// @param handler Update handler.
		void onUpdate(const UpdateHandler& handler) { _update_list.push_back(handler); };

		/// @return Element's children.
		const std::deque<std::unique_ptr<Element>>& children() const { return _elements; };
		/// @return Element's parent.
		Element* parent() const { return _parent; };

		/// @return Element position.
		DimVector& position() { return bounds.position; };
		const DimVector& position() const { return bounds.position; };
		/// @return Element size.
		DimVector& size() { return bounds.size; };
		const DimVector& size() const { return bounds.size; };
	};
};
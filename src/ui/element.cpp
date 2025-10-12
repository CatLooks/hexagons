#include "ui/element.hpp"

namespace ui {
	/// Reserves space for borders in a rectangle.
	sf::IntRect Borders::apply(sf::IntRect rect) const {
		return {
			{ rect.position.x + left, rect.position.y + top },
			{ rect.size.x - left - right, rect.size.y - top - bottom }
		};
	};

	/// Draws the element.
	void Element::drawSelf(RenderBuffer& target, sf::IntRect self) const {};
	/// Draws element children.
	void Element::drawChildren(RenderBuffer& target) const {
		for (const auto& element : _elements)
			element->draw(target);
	};

	/// Runs when the UI language gets changed.
	void Element::onTranslate() {};
	/// Runs when the element gets activated.
	void Element::onActivate() {};
	/// Runs when the element gets deactivated.
	void Element::onDeactivate() {};
	/// Runs before recalculation.
	void Element::onRecalculate() {};

	/// Virtual destructor.
	Element::~Element() {};

	/// Adds new element as a child.
	void Element::add(Element* element) {
		_elements.push_back(std::unique_ptr<Element>(element));
		element->_parent = this;
	};
	/// Removes a child element if present.
	void Element::remove(Element* element) {
		for (auto it = _elements.begin(); it != _elements.end(); it++) {
			if (it->get() == element) {
				_elements.erase(it);
				return;
			};
		};
	};
	/// Replaces old element by a new one.
	void Element::replace(Element* old, Element* repl) {
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
	/// Removes all child elements.
	void Element::clear() {
		_elements = {};
	};

	/// Recalculates draw area for the element.
	void Element::recalculate(sf::IntRect parent) {
		// ignore if not active
		if (!_active) return;

		// update before recalculation
		onRecalculate();

		// recalculate element draw areas
		_outerRect = bounds.get(parent);
		_rect = margin.apply(_outerRect);
		_innerRect = padding.apply(_rect);

		// recalculate children
		for (const auto& element : _elements)
			element->recalculate(_innerRect);
	};

	/// Draws the element and its children.
	void Element::draw(RenderBuffer& target) const {
		// ignore if not active
		if (!_active) return;

		// draw self & children
		drawSelf(target, _rect);
		drawChildren(target);
	};

	/// Updates element and its children.
	void Element::update(float delta) {
		// ignore if not active
		if (!_active) return;

		// update element
		for (const auto& handler : _update_list)
			handler(delta);

		// update children
		for (const auto& element : _elements)
			element->update(delta);
	};

	/// Emits an event to the element.
	bool Element::event(Event evt) {
		// ignore if needed
		if (!_active || ignore) return false;

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
	bool Element::handle(Event evt) {
		bool absorb = false;
		for (const auto& handler : _handle_list)
			if (handler(evt))
				absorb = true;
		return absorb;
	};

	/// Processes mouse hovering events.
	bool Element::hover(sf::Vector2i pos) {
		// store hover state
		_hover_old = _hover_now;
		_hover_now = false;

		// ignore if needed
		if (!_active || ignore) return false;

		// update children
		for (const auto& element : _elements) {
			if (element->hover(pos)) {
				if (_hover_old)
					handle((Event)Event::MouseLeave{ pos });
				return true;
			};
		};

		// update hover state
		_hover_now = _rect.contains(pos);

		// trigger mouse events
		if (_hover_now && !_hover_old) handle((Event)Event::MouseEnter{ pos });
		if (!_hover_now && _hover_old) handle((Event)Event::MouseLeave{ pos });
		return _hover_now && !transparent;
	};

	/// Attaches an event handler.
	void Element::onEvent(const EventHandler& handler) { _handle_list.push_back(handler); };
	/// Attaches an update handler.
	void Element::onUpdate(const UpdateHandler& handler) { _update_list.push_back(handler); };

	/// Updates UI language.
	void Element::translate() {
		onTranslate();
		for (auto& element : _elements)
			element->translate();
	};
	/// Activates the element.
	void Element::activate() {
		_active = true;
		onActivate();
	};
	/// Deactivates the element.
	void Element::deactivate() {
		_active = false;
		onDeactivate();
	};
	/// @return Whether the element is active.
	bool Element::active() const { return _active; };

	/// @return Element's children.
	const std::deque<std::unique_ptr<Element>>& Element::children() const { return _elements; };
	/// @return Element's parent.
	Element* Element::parent() const { return _parent; };

	/// @return Element position.
	DimVector& Element::position() { return bounds.position; };
	const DimVector& Element::position() const { return bounds.position; };
	/// @return Element size.
	DimVector& Element::size() { return bounds.size; };
	const DimVector& Element::size() const { return bounds.size; };

	/// @return Element bounding rectangle.
	sf::IntRect Element::rect() const { return _rect; };
};
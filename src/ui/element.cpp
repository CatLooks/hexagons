#include "ui/element.hpp"

namespace ui {
	/// Sets thickness in all directions.
	void Borders::set(int value) {
		left = value;
		right = value;
		top = value;
		bottom = value;
	};
	/// Sets thickness in horizontal directions.
	void Borders::setHorizontal(int value) {
		left = value;
		right = value;
	};
	/// Sets thickness in vertical directions.
	void Borders::setVertical(int value) {
		top = value;
		bottom = value;
	};

	/// Reserves space for borders in a rectangle.
	sf::IntRect Borders::apply(sf::IntRect rect) const {
		return {
			{ rect.position.x + left, rect.position.y + top },
			{ rect.size.x - left - right, rect.size.y - top - bottom }
		};
	};

	/// Constructs an iterator object.
	Element::iter::iter(iter_type a, iter_type split, iter_type b) : _it0(a), _split(split), _it1(b) {};

	/// Returns current iterator item.
	const Element::iter::value_type& Element::iter::operator*() const {
		return *((_it0 == _split) ? _it1 : _it0);
	};
	/*Element::iter::value_type& Element::iter::operator*() {
		return *((_it0 == _split) ? _it1 : _it0);
	};*/
	/// Returns current iterator item.
	const Element::iter::value_type* Element::iter::operator->() const {
		return &*((_it0 == _split) ? _it1 : _it0);
	};

	/// Advances to next value.
	void Element::iter::operator++() {
		(_it0 == _split ? _it1 : _it0)++;
	};
	/// Advances to next value.
	void Element::iter::operator++(int) {
		++*this;
	};

	/// Checks if 2 iterators point to the same item.
	bool Element::iter::operator==(const iter& oth) const {
		return _it0 == oth._it0 && _it1 == oth._it1;
	};

	/// Checks if 2 iterators point to different items.
	bool Element::iter::operator!=(const iter& oth) const {
		return !(*this == oth);
	};

	/// @return Starting iterator to all children.
	Element::iter Element::begin() const {
		return iter(_system.begin(), _system.end(), _elements.begin());
	};
	/// @return Ending iterator to all children.
	Element::iter Element::end() const {
		return iter(_system.end(), _system.end(), _elements.end());
	};

	/// Draws the element.
	void Element::drawSelf(RenderBuffer& target, sf::IntRect self) const {};
	/// Draws element children.
	void Element::drawChildren(RenderBuffer& target) const {
		for (const auto& element : *this)
			element->draw(target);
	};

	/// Runs when the UI language gets changed.
	void Element::onTranslate() {};
	/// Runs when the element gets activated.
	void Element::onActivate() {};
	/// Runs when the element gets deactivated.
	void Element::onDeactivate() {};

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
		_elements.clear();
	};

	/// Adds new system element as a child.
	void Element::adds(Element* element) {
		_system.push_back(std::unique_ptr<Element>(element));
		element->_parent = this;
	};
	/// Removes a system element if present.
	void Element::removes(Element* element) {
		for (auto it = _system.begin(); it != _system.end(); it++) {
			if (it->get() == element) {
				_system.erase(it);
				return;
			};
		};
	};
	/// Replaces old system element by a new one.
	void Element::replaces(Element* old, Element* repl) {
		// try replacing old element
		for (auto it = _system.begin(); it != _system.end(); it++) {
			if (it->get() == old) {
				*it = std::unique_ptr<Element>(repl);
				repl->_parent = this;
				return;
			};
		};

		// add if old was not found
		adds(repl);
	};
	/// Removes all system elements.
	void Element::clears() {
		_system.clear();
	};

	/// Pushes a new animation.
	void Element::push(Anim* anim) {
		_anims.push_back(std::unique_ptr<Anim>(anim));
	};
	/// @return Whether the element has any animations running.
	bool Element::animated() const {
		return !_anims.empty();
	};

	/// Recalculates draw area for the element.
	void Element::recalculate(const sf::Time& delta, sf::IntRect parent) {
		// ignore if not active
		if (!_active) return;

		// update before recalculation
		for (const auto& handler : _recalc_list)
			handler(delta);

		// update animations
		auto it = _anims.begin();
		while (it != _anims.end()) {
			// tick animator
			(*it)->update(delta);
			if ((*it)->active())
				// go to next animator
				it++;
			else
				// delete animator from list
				it = _anims.erase(it);
		};

		// recalculate element draw areas
		_outerRect = bounds.get(parent);
		_rect = margin.apply(_outerRect);
		_innerRect = padding.apply(_rect);

		// recalculate children
		for (const auto& element : _elements)
			element->recalculate(delta, _innerRect);
	};

	/// Draws the element and its children.
	void Element::draw(RenderBuffer& target) const {
		// ignore if not active
		if (!_active) return;

		// forward buffer
		if (forwarding) target.forward();

		// draw self & children
		drawSelf(target, _rect);
		drawChildren(target);
	};

	/// Updates element and its children.
	void Element::update(const sf::Time& delta) {
		// ignore if not active
		if (!_active) return;

		// update element
		for (const auto& handler : _update_list)
			handler(delta);

		// update children
		for (const auto& element : *this)
			element->update(delta);
	};

	/// Emits an event to the element.
	bool Element::event(Event evt) {
		// ignore if needed
		if (!_active || ignore) return false;

		// send event to children
		for (const auto& element : *this)
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
		for (const auto& element : *this) {
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
	/// Attaches an update handler.
	void Element::onRecalculate(const UpdateHandler& handler) { _recalc_list.push_back(handler); };

	/// Updates UI language.
	void Element::translate() {
		onTranslate();
		for (auto& element : *this)
			element->translate();
	};
	/// Activates the element.
	void Element::activate() {
		_active = true;
		onActivate();
		for (auto& element : *this)
			element->activate();
	};
	/// Deactivates the element.
	void Element::deactivate() {
		_active = false;
		onDeactivate();
		for (auto& element : *this)
			element->deactivate();
	};
	/// @return Whether the element is active.
	bool Element::active() const { return _active; };

	/// @return Element's children.
	const std::list<std::unique_ptr<Element>>& Element::children() const { return _elements; };
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
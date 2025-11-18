#include "ui/drag.hpp"
#include "ui/window.hpp"
#include "mathext.hpp"

namespace ui {
	/// Constructs an inactive drag event.
	Drag::Drag(sf::Vector2i* ref): _ref(ref) {};

	/// Starts the drag event.
	void Drag::start(sf::Vector2i mouse) {
		_mouse = mouse;
		_value = *_ref;
		_active = true;
	};

	/// Stops the drag event.
	void Drag::stop() {
		_active = false;
	};

	/// Updates the dragged value.
	void Drag::move(sf::Vector2i mouse) {
		// ignore if not active
		if (!_active) return;

		// calculate new value
		sf::Vector2i diff = {
			int((mouse.x - _mouse.x) * _scale),
			int((mouse.y - _mouse.y) * _scale)
		};
		sf::Vector2i value = invert ? (_value - diff) : (_value + diff);

		// clamp value
		if (max) {
			value.x = ext::imin(value.x, max->x - 1);
			value.y = ext::imin(value.y, max->y - 1);
		};
		if (min) {
			value.x = ext::imax(value.x, min->x);
			value.y = ext::imax(value.y, min->y);
		};

		// set reference to new value
		*_ref = value;
	};

	/// Sets drag multiplier.
	void Drag::scaling(float scale) {
		_scale = scale;
	};

	/// Updates the drag event.
	void Drag::update(sf::Vector2i mouse, sf::Vector2i window, bool pressed) {
		// check for press
		if (!_pressed && pressed)
			if (sf::IntRect({}, ui::window.size()).contains(mouse))
				start(mouse);

		// update drag
		move(mouse);

		// check for release
		if (_pressed && !pressed)
			stop();

		// update button status
		_pressed = pressed;
	};
};
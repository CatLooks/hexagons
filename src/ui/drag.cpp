#include "ui/drag.hpp"
#include "ui/window.hpp"

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
			if (value.x >= max->x) value.x = max->x - 1;
			if (value.y >= max->y) value.y = max->y - 1;
		};
		if (min) {
			if (value.x < min->x) value.x = min->x;
			if (value.y < min->y) value.y = min->y;
		};

		// set reference to new value
		*_ref = value;
	};

	/// Updates the drag event.
	void Drag::update(sf::Vector2i mouse, bool pressed) {
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

	/// Returns current drag scaling.
	float Drag::getScale() const {
		return _scale;
	};

	/// Updates drag scaling.
	void Drag::setScale(float scale, sf::Vector2i mouse) {
		// @todo: fix this fucking thing
		//
		// in theory this should be:
		// 
		//     camera = (camera + mouse) * new_scale / old_scale - mouse;
		//
		// reference vector is either not modified
		// or constantly gets overwritten by inner state of ui::Drag
		//
		// also code above does not work

		// set new scale
		_scale = scale;
	};

	/// Updates drag scaling with a mouse wheel.
	void Drag::scrollScale(float multiplier, int wheel, sf::Vector2i mouse) {
		// equivalent to
		// scale *= multiplier ^ wheel

		float scale = _scale;
		while (wheel > 0) {
			scale *= multiplier;
			wheel--;
		};
		while (wheel < 0) {
			scale /= multiplier;
			wheel++;
		};
		setScale(scale, mouse);
	};
};
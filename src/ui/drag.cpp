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
		// clamp scale
		scale = ext::fclamp(scale, min_zoom, max_zoom);

		// shift camera
		*_ref = sf::Vector2i(sf::Vector2f(*_ref + mouse) * scale / _scale) - mouse;

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
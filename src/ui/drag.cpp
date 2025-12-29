#include "ui/drag.hpp"
#include "ui/window.hpp"
#include "mathext.hpp"

namespace ui {
	/// Constructs an inactive drag event.
	Drag::Drag(const Set& setter): _set(setter) {};

	/// Starts the drag event.
	void Drag::start(sf::Vector2i mouse) {
		_mouse = mouse;
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

		// calculate offset
		sf::Vector2f diff = {
			float(mouse.x - _mouse.x) * _scale,
			float(mouse.y - _mouse.y) * _scale
		};

		// apply offset
		_set(invert ? -diff : diff);

		// rebase mouse position
		_mouse = mouse;
	};

	/// Sets drag multiplier.
	void Drag::scaling(float scale) {
		_scale = scale;
	};

	/// Updates the drag event.
	void Drag::update(sf::Vector2i mouse, bool pressed) {
		// update drag
		move(mouse);

		// check for release
		if (_pressed && !pressed)
			stop();

		// update button status
		_pressed = pressed;
	};
};
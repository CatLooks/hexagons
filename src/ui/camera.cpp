#include "ui/camera.hpp"
#include "mathext.hpp"

namespace ui {
	/// Constructs a camera object.
	Camera::Camera(Layer* layer, sf::Vector2i* camera, float scroll)
		: _layer(layer), _drag(camera), scrollPower(scroll)
	{
		_drag.invert = true;
	};

	/// Controls camera panning.
	void Camera::pan(bool pressed, sf::Vector2i mouse, sf::Vector2i window) {
		_drag.update(mouse, window, pressed);
	};

	/// Controls camera zoom.
	void Camera::scroll(int wheel, sf::Vector2i mouse, sf::Vector2i window) {
		// get new zoom value
		float zoom = ext::fclamp(
			ext::fpown(scrollPower, wheel) * _zoom,
			minZoom, maxZoom
		);

		// recalculate camera
		sf::Vector2i& camera = *_drag._ref;
		//camera = window / 2 - sf::Vector2i(sf::Vector2f(window / 2 - camera) * zoom / _zoom);

		// set new zoom
		_zoom = zoom;
		_drag._scale = zoom;
	};

	/// @return Current zoom.
	float Camera::zoom() const {
		return _zoom;
	};
};
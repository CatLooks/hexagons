#include "ui/camera.hpp"
#include "mathext.hpp"

namespace ui {
	/// Starts camera position drag event.
	void Camera::start(sf::Vector2i mouse) {
		_drag.start(mouse);
	};

	/// Stops camera position drag event.
	void Camera::stop() {
		_drag.stop();
	};

	/// Moves the camera by an offset.
	void Camera::move(sf::Vector2f offset) {
		_cam += offset;
	};

	/// Sets new camera position.
	void Camera::setPosition(sf::Vector2f pos) {
		_cam = pos;
	};

	/// Updates the camera position.
	void Camera::update(sf::Vector2i mouse, bool pressed) {
		_drag.update(mouse, pressed);
	};

	/// Constructs a camera object.
	Camera::Camera(Layer* layer, float scroll)
		: _drag([=](sf::Vector2f diff) { _cam += diff; }), _layer(layer), scrollPower(scroll)
	{
		_drag.invert = true;
	};

	/// Controls camera zoom.
	void Camera::scroll(int wheel, sf::Vector2i mouse) {
		setZoom(ext::fclamp(
			ext::fpown(scrollPower, wheel) * _drag._scale,
			minZoom, maxZoom
		));
	};

	/// Sets a new zoom value.
	void Camera::setZoom(float zoom) {
		_drag.scaling(zoom);
	};

	/// @return Current zoom.
	float Camera::zoom() const {
		return _drag._scale;
	};

	/// Returns camera view rectangle.
	sf::FloatRect Camera::view(sf::Vector2i window) const {
		auto size = (sf::Vector2f)window * _drag._scale;
		return { _cam - size * 0.5f, size };
	};
};
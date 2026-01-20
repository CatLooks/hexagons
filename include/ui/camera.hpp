#pragma once

// include dependencies
#include <limits>
#include "drag.hpp"
#include "layer.hpp"

namespace ui {
	/// Layer camera control object.
	class Camera {
	protected:
		Drag        _drag; /// Drag event.
		sf::Vector2f _cam; /// Camera position.
		Layer*     _layer; /// Controlled layer.

	public:
		float minZoom = 0.f;                                   /// Minimal zoom value.
		float maxZoom = std::numeric_limits<float>::infinity(); /// Maximal zoom value.
		float scrollPower;                                     /// Scrolling coefficient.

	public:
		/// Starts camera position drag event.
		/// 
		/// @param mouse Mouse position.
		void start(sf::Vector2i mouse);

		/// Stops camera position drag event.
		void stop();

		/// Moves the camera by an offset.
		/// 
		/// @param offset Camera offset.
		void move(sf::Vector2f offset);

		/// Sets new camera position.
		/// 
		/// @param pos Camera position.
		void setPosition(sf::Vector2f pos);

		/// Updates the camera position.
		/// 
		/// @param mouse Mouse position.
		/// @param pressed Whether the drag button is pressed.
		void update(sf::Vector2i mouse, bool pressed);

	public:
		/// Constructs a camera object.
		/// 
		/// @param layer Layer reference.
		/// @param scroll Scroll coefficient.
		Camera(Layer* layer, float scroll);

		/// Controls camera zoom.
		/// 
		/// @param wheel Mouse wheel delta.
		/// @param mouse Mouse position.
		void scroll(int wheel, sf::Vector2i mouse);

		/// Sets a new zoom value.
		/// 
		/// @param zoom Zoom value.
		void setZoom(float zoom);
		/// Returns current zoom value.
		float zoom() const;

		/// Returns camera view rectangle.
		///
		/// @param window Window size.
		sf::FloatRect view(sf::Vector2i window) const;
	};
};
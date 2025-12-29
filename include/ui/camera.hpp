#pragma once

// include dependencies
#include "drag.hpp"
#include "layer.hpp"

namespace ui {
	/// Layer camera control object.
	class Camera {
	protected:
		Layer* _layer;     /// Controlled layer.
		Drag _drag;        /// Drag event.
		float _zoom = 1.f; /// Layer zoom.

	public:
		float minZoom = 0.f;      /// Minimal zoom value.
		float maxZoom = INFINITY; /// Maximal zoom value.
		float scrollPower;        /// Scrolling coefficient.

	public:
		/// Constructs a camera object.
		/// 
		/// @param layer Layer reference.
		/// @param camera Camera position reference.
		/// @param scroll Scroll coefficient.
		Camera(Layer* layer, sf::Vector2i* camera, float scroll);

		/// Controls camera panning.
		/// 
		/// @param pressed Whether the camera button is pressed.
		/// @param mouse Mouse position.
		/// @param window Window size.
		void pan(bool pressed, sf::Vector2i mouse, sf::Vector2i window);

		/// Controls camera zoom.
		/// 
		/// @param wheel Mouse wheel delta.
		/// @param mouse Mouse position.
		/// @param window Window size.
		void scroll(int wheel, sf::Vector2i mouse, sf::Vector2i window);

		/// Sets a new zoom value.
		/// 
		/// @param zoom Zoom value.
		void set(float zoom);

		/// @return Current zoom.
		float zoom() const;
	};
};
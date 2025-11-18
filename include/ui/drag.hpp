#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <optional>

namespace ui {
	class Camera;

	/// Mouse drag event manager.
	class Drag {
		friend Camera;

	protected:
		sf::Vector2i _mouse; /// Initial mouse position.
		sf::Vector2i _value; /// Initial value.
		sf::Vector2i*  _ref; /// Drag value setter.

		float    _scale = 1.f; /// Default drag scaling.
		bool  _active = false; /// Whether the event is active.
		bool _pressed = false; /// Whether the mouse is pressed.

	public:
		bool invert = false; /// Inverts the drag direction.

		std::optional<sf::Vector2i> min = {}; /// Minimal drag values for both axes.
		std::optional<sf::Vector2i> max = {}; /// First out-of-range drag values for both axes.

	protected:
		/// Starts the drag event.
		/// 
		/// Uses current dragged value for initialization.
		/// 
		/// @param mouse Initial mouse position.
		void start(sf::Vector2i mouse);
		/// Stops the drag event.
		void stop();
		/// Updates the dragged value.
		/// 
		/// @param mouse Current mouse position.
		void move(sf::Vector2i mouse);

	public:
		/// Constructs an inactive drag event.
		///
		/// @param ref Dragged value reference.
		Drag(sf::Vector2i* ref);

		/// Sets drag multiplier.
		/// 
		/// @param scale Drag multiplier.
		void scaling(float scale);

		/// Updates the drag event.
		/// 
		/// @param mouse Mouse position.
		/// @param window Window size.
		/// @param pressed Whether the drag button is pressed.
		void update(sf::Vector2i mouse, sf::Vector2i window, bool pressed);
	};
};
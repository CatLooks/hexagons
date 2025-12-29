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

	public:
		/// Drag value setter.
		///
		/// @param offset Value offset since last frame.
		using Set = std::function<void(sf::Vector2f offset)>;

	protected:
		Set _set; /// Dragged value setter.

		sf::Vector2i _mouse; /// Initial mouse position.

		float    _scale = 1.f; /// Default drag scaling.
		bool  _active = false; /// Whether the event is active.
		bool _pressed = false; /// Whether the mouse is pressed.

	public:
		bool invert = false; /// Inverts the drag direction.

		/// Starts the drag event.
		/// 
		/// Uses current dragged value for initialization.
		/// 
		/// @param mouse Initial mouse position.
		void start(sf::Vector2i mouse);
		/// Stops the drag event.
		void stop();

	protected:
		/// Updates the dragged value.
		/// 
		/// @param mouse Current mouse position.
		void move(sf::Vector2i mouse);

	public:
		/// Constructs an inactive drag event.
		///
		/// @param setter Dragged value setter function.
		Drag(const Set& setter);

		/// Sets drag multiplier.
		/// 
		/// @param scale Drag multiplier.
		void scaling(float scale);

		/// Updates the drag event.
		/// 
		/// @param mouse Mouse position.
		/// @param pressed Whether the drag button is pressed.
		void update(sf::Vector2i mouse, bool pressed);
	};
};
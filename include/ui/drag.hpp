#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <optional>

namespace ui {
	/// Mouse drag event manager.
	class Drag {
	protected:
		sf::Vector2i _mouse; /// Initial mouse position.
		sf::Vector2i _value; /// Initial value.
		sf::Vector2i*  _ref; /// Drag value setter.
		float  _scale = 1.f; /// Drag scale value.
		bool  _active = false; /// Whether the event is active.
		bool _pressed = false; /// Whether the mouse is pressed.

	public:
		bool                     invert = false; /// Inverts the drag direction.
		std::optional<sf::Vector2i> min = {};    /// Minimal drag values for both axes.
		std::optional<sf::Vector2i> max = {};    /// First out-of-range drag values for both axes.

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

		/// Updates the drag event.
		/// 
		/// @param mouse Mouse position.
		/// @param pressed Whether the drag button is pressed.
		void update(sf::Vector2i mouse, bool pressed);

		/// Returns current drag scaling.
		float getScale() const;
		/// Updates drag scaling.
		/// 
		/// @param scale Scaling factor.
		/// @param mouse Mouse position.
		void setScale(float scale, sf::Vector2i mouse);

		/// Updates drag scaling with a mouse wheel.
		/// 
		/// Positive `wheel` values increase scaling.
		/// 
		/// Negative `wheel` values decrease scaling.
		/// 
		/// @param multiplier Multiplier for each scroll.
		/// @param wheel Amount of wheel scrolls.
		/// @param mouse Mouse position.
		void scrollScale(float multiplier, int wheel, sf::Vector2i mouse);
	};
};
#pragma once

// include dependencies
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <optional>
#include <variant>

namespace ui {
	/// @brief UI event object.
	class Event {
	public:
		/// @brief Fires when mouse is moved.
		struct MouseMove {
			sf::Vector2i position; /// Mouse position.
		};
		/// @brief Fires when mouse first starts hovering over the element.
		struct MouseEnter : MouseMove {};
		/// @brief Fires when mouse first stops hovering over the element.
		struct MouseLeave : MouseMove {};

		/// @brief Fires when mouse button is being held down.
		struct MouseDown : MouseMove {
			sf::Mouse::Button button; /// Active mouse key.
		};
		/// @brief Fires when mouse button gets pressed.
		struct MousePress : MouseDown {};
		/// @brief Fires when mouse button gets released.
		struct MouseRelease : MouseDown {};

		/// @brief Fires when a key is being held down.
		struct KeyDown {
			sf::Keyboard::Key key; /// Active key.
		};
		/// @brief Fires when a key gets pressed.
		struct KeyPress : KeyDown {
			bool alt;     /// Whether the ALT key is being pressed.
			bool shift;   /// Whether the SHIFT key is being pressed.
			bool control; /// Whether the CTRL key is being pressed.
			bool system;  /// Whether the system key is being pressed.
		};
		/// @brief Fires when a key gets released.
		struct KeyRelease : KeyPress {};

		/// @brief Variant of all events.
		using Variant = std::variant<
			MouseMove,
			MouseEnter,
			MouseLeave,
			MouseDown,
			MousePress,
			MouseRelease,
			KeyDown,
			KeyPress,
			KeyRelease
		>;

	private:
		Variant data; /// Event data.

	public:
		/// Constructs an event with specified data.
		/// @param data Event data.
		Event(Variant data) : data(data) {};

		/// @return Event data if event type is `T`, `nullopt` otherwise.
		/// @tparam T Event type.
		template <typename T> std::optional<T> get() const {
			if (const T* ptr = std::get_if<T>(&data))
				return *ptr;
			return {};
		};

		/// @return Whether the event is of type `T`.
		/// @tparam T Event type.
		template <typename T> bool is() const {
			return std::get_if<T>(&data) != nullptr;
		};

		/// @return Event mouse position if event is mouse related.
		std::optional<sf::Vector2i> mouse() const {
			if (const auto ptr = get<MouseMove>()) return ptr->position;
			if (const auto ptr = get<MouseEnter>()) return ptr->position;
			if (const auto ptr = get<MouseLeave>()) return ptr->position;
			if (const auto ptr = get<MouseDown>()) return ptr->position;
			if (const auto ptr = get<MousePress>()) return ptr->position;
			if (const auto ptr = get<MouseRelease>()) return ptr->position;
			return {};
		};
	};
};
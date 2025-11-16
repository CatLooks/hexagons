#pragma once

// include dependencies
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>
#include "layer.hpp"

namespace ui {
	/// Window object.
	/// Provides window and UI management.
	class Window {
	private:
		/// SFML window handle.
		sf::RenderWindow _win;
		/// Event queue for interface.
		std::deque<sf::Event> _evtq;
		/// User interface.
		Interface _itf;

	public:
		/// Returns a reference to window interface object.
		Interface& interface();

		/// Creates new window.
		///
		/// @param size Window size.
		void create(sf::Vector2u size);

		/// Sets new window title.
		/// 
		/// @param str Window title.
		void title(const std::string& str);

		/// Closes the window.
		void close();

		/// Checks whether the window is active.
		bool active();

		/// Fetches and processes window events.
		/// @return List of unprocessed events.
		const std::deque<sf::Event>& events();

		/// Updates interface and draws a new frame.
		void frame();

	public:
		/// Returns current window size.
		sf::Vector2i size() const;

		/// Returns current mouse position.
		sf::Vector2i mouse() const;
	};

	/// Main window.
	extern ui::Window window;
};
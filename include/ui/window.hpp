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
		/// Whether the window is fullscreen.
		bool _full = false;

	public:
		/// Returns a reference to window interface object.
		Interface& interface();

		/// Creates new window.
		///
		/// @param size Window size.
		/// @param fullscreen Whether the fullscreen the window.
		void create(sf::Vector2u size, bool fullscreen);

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

		/// Checks whether the window is focused.
		bool focused() const;
		/// Checks whether the window is fullscreen.
		bool fullscreen() const;
	};

	/// Main window.
	extern ui::Window window;
};
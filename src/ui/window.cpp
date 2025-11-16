#include "ui/window.hpp"
#include <SFML/Graphics.hpp>

namespace ui {
	/// Returns a reference to window interface object.
	Interface& Window::interface() {
		return _itf;
	};

	/// Creates new window.
	void Window::create(sf::Vector2u size) {
		// window settings
		sf::ContextSettings settings;
		settings.antiAliasingLevel = 2;

		// create window
		_win.create(sf::VideoMode(size), "Hexagons", sf::State::Windowed, settings);

		// configure window
		//_win.setVerticalSyncEnabled(true);
		_win.setFramerateLimit(60);
		_win.setKeyRepeatEnabled(false);
	};

	/// Sets new window title.
	void Window::title(const std::string& str) {
		_win.setTitle(str);
	};

	/// Closes the window.
	void Window::close() {
		_win.close();
	};

	/// Checks whether the window is active.
	bool Window::active() {
		return _win.isOpen();
	};

	/// Fetches and processes window events.
	const std::deque<sf::Event>& Window::events() {
		// process window events
		while (const auto event = _win.pollEvent()) {
			// check for window close
			if (event->is<sf::Event::Closed>()) {
				close();
				continue;
			};

			// pass event to queue
			_evtq.push_back(*event);
		};

		// return queue reference
		return _evtq;
	};

	/// Updates interface and draws a new frame.
	void Window::frame() {
		// recalculate & update interface
		_itf.recalculate(_win.getSize());
		_itf.eventq(_evtq);
		_itf.update(sf::Mouse::getPosition(_win));

		// draw interface
		_itf.draw(_win);
		_win.display();
	};

	/// Returns current window size.
	sf::Vector2i Window::size() const {
		return (sf::Vector2i)_win.getSize();
	};

	/// Returns current mouse position.
	sf::Vector2i Window::mouse() const {
		return sf::Mouse::getPosition(_win);
	};

	/// Main window.
	ui::Window window;
};
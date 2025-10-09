#pragma once

// include dependencies
#include <SFML/Graphics.hpp>
#include "element.hpp"
#include <deque>

namespace ui {
	/// Interface layer element.
	/// Spans the entire window.
	class Layer : public Element {
	protected:
		/// Resets layer bounds.
		void preRecalc() override {
			bounds = { 0px, 0px, 1ps, 1ps };
		};
	};

	/// Interface container.
	class Interface {
	private:
		/// Interface layers.
		std::deque<std::unique_ptr<Layer>> layers;
		/// Window rectangle.
		sf::IntRect winRect;

		/// Sends the event to all interface layers.
		/// @param evt Sent event.
		void send_event(const Event& evt) {
			for (auto& layer : layers)
				layer->event(evt);
		};

	public:
		/// Creates a new interface layer.
		/// @return Reference to new layer.
		Layer* newLayer() {
			layers.push_back(std::unique_ptr<Layer>(new Layer));
			return layers.back().get();
		};

		/// Recalculates interface.
		/// @param windowSize Window size.
		void recalculate(sf::Vector2u windowSize) {
			// update window rectangle
			winRect = { {}, (sf::Vector2i)windowSize };

			// recalculate each layer
			for (auto& layer : layers)
				layer->recalculate(winRect);
		};

		/// Send an event to interface.
		void event(const sf::Event& evt) {
			// check for keyboard events
			if (const auto* data = evt.getIf<sf::Event::KeyPressed>()) {
				send_event((Event)Event::KeyPress{ data->code });
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::KeyReleased>()) {
				send_event((Event)Event::KeyRelease{ data->code });
				return;
			};

			// check for mouse button events
			if (const auto* data = evt.getIf<sf::Event::MouseButtonPressed>()) {
				send_event((Event)Event::MousePress{ data->position, data->button });
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseButtonReleased>()) {
				send_event((Event)Event::MouseRelease{ data->position, data->button });
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseMoved>()) {
				send_event((Event)Event::MouseMove{ data->position });
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseWheelScrolled>()) {
				send_event((Event)Event::MouseWheel{ data->position, data->delta });
				return;
			};
		};

		/// Updates the interface.
		/// @param win Main window.
		/// @param delta Time elapsed since last frame.
		void update(sf::Window& win, float delta) {
			sf::Vector2i mouse = sf::Mouse::getPosition(win);

			// generate static mouse events
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				send_event((Event)Event::MouseDown{ mouse, sf::Mouse::Button::Left });
			};

			// update each layer
			for (auto& layer : layers) {
				layer->hover(mouse);
				layer->update(delta);
			};
		};

		/// Draws the interface.
		void draw() {
			for (const auto& layer : layers)
				layer->draw();
		};
	};
};
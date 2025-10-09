#pragma once

// include dependencies
#include "element.hpp"
#include <deque>

namespace ui {
	class Interface;

	/// Interface layer element.
	/// Spans the entire window.
	class Layer : public Element {
		friend Interface;

	protected:
		/// Layer render buffer.
		RenderBuffer _buffer;
		/// View override.
		std::optional<sf::View> _view;

		/// Prepare for layer recalculation.
		void recalcUpdate() override {
			// reset layer bounds (just in case)
			bounds = { 0px, 0px, 1ps, 1ps };
			// clear render buffer
			_buffer.clear();
		};

	public:
		/// Constructs a new layer.
		/// @param texture Layer rendering texture.
		Layer(const sf::Texture* texture) : _buffer({ texture }) {};

		/// Sets new rendering texture.
		void setTexture(const sf::Texture* texture) {
			_buffer.states().texture = texture;
		};
		/// Sets new rendering shader.
		void setTexture(const sf::Shader* shader) {
			_buffer.states().shader = shader;
		};
		/// Sets rendering view override.
		/// 
		/// @param view Optional view override.
		void setView(std::optional<sf::View> view) {
			_view = view;
		}

		/// Renders drawn layer onto render target.
		/// 
		/// @param target Render target.
		/// @param default_view Default view.
		/// 
		/// @return Rendered triangle count.
		RenderStats render(sf::RenderTarget& target, const sf::View& default_view) const {
			target.setView(_view ? *_view : default_view);
			return _buffer.draw(target);
		};
	};

	/// Interface container.
	class Interface {
	private:
		/// Interface layers.
		std::deque<std::unique_ptr<Layer>> layers;
		/// Interface render statistics renderer.
		std::function<void(sf::RenderTarget&, const RenderStats&)> info;
		/// Window rectangle.
		sf::IntRect winRect;
		/// Default view.
		sf::View view;
		/// Delta clock.
		sf::Clock clock;

		/// Sends the event to all interface layers.
		/// @param evt Sent event.
		void send_event(const Event& evt) {
			for (auto& layer : layers)
				layer->event(evt);
		};

	public:
		/// Creates a new interface layer.
		/// 
		/// @param texture Layer rendering texture.
		/// 
		/// @return Reference to new layer.
		Layer* layer(const sf::Texture* texture) {
			layers.push_back(std::unique_ptr<Layer>(new Layer(texture)));
			return layers.back().get();
		};

		/// Recalculates interface.
		/// @param windowSize Window size.
		void recalculate(sf::Vector2u windowSize) {
			// update window rectangle
			winRect = { {}, (sf::Vector2i)windowSize };
			// update default view
			view = sf::View(sf::FloatRect{ {}, (sf::Vector2f)windowSize });

			// recalculate each layer
			for (auto& layer : layers)
				layer->recalculate(winRect);
		};

		/// Send an event to interface.
		void event(const sf::Event& evt) {
			// check for keyboard events
			if (const auto* data = evt.getIf<sf::Event::KeyPressed>())
				return send_event((Event)Event::KeyPress{ data->code });
			if (const auto* data = evt.getIf<sf::Event::KeyReleased>())
				return send_event((Event)Event::KeyRelease{ data->code });

			// check for mouse button events
			if (const auto* data = evt.getIf<sf::Event::MouseButtonPressed>())
				return send_event((Event)Event::MousePress{ data->position, data->button });
			if (const auto* data = evt.getIf<sf::Event::MouseButtonReleased>())
				return send_event((Event)Event::MouseRelease{ data->position, data->button });
			if (const auto* data = evt.getIf<sf::Event::MouseMoved>())
				return send_event((Event)Event::MouseMove{ data->position });
			if (const auto* data = evt.getIf<sf::Event::MouseWheelScrolled>())
				return send_event((Event)Event::MouseWheel{ data->position, data->delta });
		};

		/// Updates the interface.
		/// @param win Main window.
		void update(sf::Window& win) {
			sf::Vector2i mouse = sf::Mouse::getPosition(win);

			// calculate delta
			float delta = clock.restart().asSeconds();

			// update each layer
			for (auto& layer : layers) {
				layer->hover(mouse);
				layer->update(delta);
			};
		};

		/// Draws the interface.
		/// 
		/// @param target Render target.
		/// @param def_view Default view.
		void draw(sf::RenderTarget& target) {
			RenderStats stats;

			// render layers
			for (auto& layer : layers) {
				layer->draw(layer->_buffer);
				stats |= layer->render(target, view);
			};

			// render stats
			if (info) info(target, stats);
		};

		/// Sets rendering statistics rendering callback for the interface.
		/// 
		/// @param target Render target.
		/// @param stats Rendering statistics.
		void setStatDrawCall(std::function<void(sf::RenderTarget& target, const RenderStats& stats)> call) {
			info = call;
		};
	};
};
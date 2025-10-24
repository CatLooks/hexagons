#include "ui/layer.hpp"

namespace ui {
	/// Constructs a new layer.
	Layer::Layer(const sf::Texture* texture): _buffer({ texture }) {
		onRecalculate([=](const sf::Time& _) {
			// reset layer bounds (just in case)
			bounds = { 0px, 0px, 1ps, 1ps };
			// clear render buffer
			_buffer.clear();
		});
	};

	/// Sets new rendering texture.
	void Layer::setTexture(const sf::Texture* texture) {
		_buffer.states().texture = texture;
	};
	/// Sets new rendering shader.
	void Layer::setShader(const sf::Shader* shader) {
		_buffer.states().shader = shader;
	};
	/// Sets rendering view override.
	void Layer::setView(std::optional<sf::View> view) {
		_view = view;
	}

	/// Renders drawn layer onto render target.
	RenderStats Layer::render(sf::RenderTarget& target, const sf::View& default_view) const {
		target.setView(_view ? *_view : default_view);
		return _buffer.draw(target);
	};

	/// Sends the event to all interface layers.
	void Interface::send_event(const Event& evt) {
		for (auto& layer : layers)
			layer->event(evt);
	};

	/// Creates a new interface layer.
	Layer* Interface::layer(const sf::Texture* texture) {
		layers.push_back(std::unique_ptr<Layer>(new Layer(texture)));
		return layers.back().get();
	};

	/// Recalculates interface.
	void Interface::recalculate(sf::Vector2u windowSize) {
		// update window rectangle
		winRect = { {}, (sf::Vector2i)windowSize };
		// update default view
		view = sf::View(sf::FloatRect{ {}, (sf::Vector2f)windowSize });

		// calculate delta
		sf::Time delta = anim_clock.restart();

		// recalculate each layer
		for (auto& layer : layers)
			layer->recalculate(delta, winRect);
	};

	/// Send an event to interface.
	void Interface::event(const sf::Event& evt) {
		// check for keyboard events
		if (const auto* data = evt.getIf<sf::Event::KeyPressed>())
			return send_event((Event)Event::KeyPress{ data->code, data->scancode, data->alt, data->shift, data->control, data->system });
		if (const auto* data = evt.getIf<sf::Event::KeyReleased>())
			return send_event((Event)Event::KeyRelease{ data->code, data->scancode, data->alt, data->shift, data->control, data->system });

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
	void Interface::update(sf::Vector2i mouse) {
		// calculate delta
		sf::Time delta = upd_clock.restart();

		// update each layer
		for (auto& layer : layers) {
			layer->hover(mouse);
			layer->update(delta);
		};
	};

	/// Draws the interface.
	void Interface::draw(sf::RenderTarget& target) {
		RenderStats stats;

		// render layers
		for (auto& layer : layers) {
			layer->draw(layer->_buffer);
			stats |= layer->render(target, view);
		};

		// render stats
		if (info) info(target, stats);
	};

	/// Updates interface language.
	void Interface::translate() const {
		for (const auto& layer : layers)
			layer->translate();
	};

	/// Sets rendering statistics rendering callback for the interface.
	void Interface::setStatDrawCall(std::function<void(sf::RenderTarget& target, const RenderStats& stats)> call) {
		info = call;
	};
};
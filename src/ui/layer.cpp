#include "ui/layer.hpp"
#include <SFML/Graphics/Sprite.hpp>

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

	/// Configures intermediate rendering.
	void Layer::setArea(sf::Vector2u size, sf::IntRect area) {
		sf::Vector2f fact = sf::Vector2f(area.size).componentWiseDiv((sf::Vector2f)size);
		_ir = ir_t(sf::RenderTexture(size), area, fact, { 1.f / fact.x, 1.f / fact.y });
	};
	/// Removes an intermediate texture step.
	void Layer::removeArea() {
		_ir = {};
	};

	/// Returns layer texture size.
	sf::IntRect Layer::view(sf::IntRect window) const {
		return _ir ? sf::IntRect({}, (sf::Vector2i)_ir->tex.getSize()) : window;
	};

	/// Maps a screen-space position into a layer position.
	sf::Vector2i Layer::map(sf::Vector2i pos) const {
		if (!_ir) return pos;
		return (sf::Vector2i)(sf::Vector2f(pos - _ir->area.position).componentWiseMul(_ir->inv_fact));
	};

	/// Renders drawn layer onto render target.
	RenderStats Layer::render(sf::RenderTarget& target) {
		// draw buffer to render target
		auto stats = _buffer.draw(_ir ? _ir->tex : target);
		if (_ir) {
			// update intermediate sprite
			sf::Sprite spr(_ir->tex.getTexture());
			spr.setPosition((sf::Vector2f)_ir->area.position);
			spr.setScale(_ir->fact);

			// render intermediate to target
			_ir->tex.display();
			target.draw(spr);
			stats.inters++;

			// clear textuer
			_ir->tex.clear(sf::Color::Transparent);
		};
		return stats;
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
		// update view
		view = sf::View(sf::FloatRect{ {}, (sf::Vector2f)windowSize });

		// calculate delta
		sf::Time delta = anim_clock.restart();

		// recalculate each layer
		for (auto& layer : layers) {
			sf::IntRect view = layer->view(winRect);
			layer->recalculate(delta, view);
		};
	};

	/// Send an event to interface.
	void Interface::event(const sf::Event& evt) {
		for (auto& layer : layers) {
			// check for keyboard events
			if (const auto* data = evt.getIf<sf::Event::KeyPressed>()) {
				layer->event((Event)Event::KeyPress{ data->code, data->scancode, data->alt, data->shift, data->control, data->system });
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::KeyReleased>()) {
				layer->event((Event)Event::KeyRelease{ data->code, data->scancode, data->alt, data->shift, data->control, data->system });
				return;
			};

			// check for mouse button events
			if (const auto* data = evt.getIf<sf::Event::MouseButtonPressed>()) {
				layer->event((Event)Event::MousePress{ layer->map(data->position), data->button });
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseButtonReleased>()) {
				layer->event((Event)Event::MouseRelease{ layer->map(data->position), data->button });
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseMoved>()) {
				layer->event((Event)Event::MouseMove{ layer->map(data->position) });
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseWheelScrolled>()) {
				layer->event((Event)Event::MouseWheel{ layer->map(data->position), data->delta });
				return;
			};
		};
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
		target.setView(view);
		for (auto& layer : layers) {
			layer->draw(layer->_buffer);
			stats |= layer->render(target);
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
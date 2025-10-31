#include "ui/layer.hpp"
#include <SFML/Graphics/Sprite.hpp>

namespace ui {
	/// Constructs a new layer.
	Layer::Layer() {
		onRecalculate([=](const sf::Time& _) {
			// reset layer bounds (just in case)
			bounds = { 0px, 0px, 1ps, 1ps };
			// clear render buffer
			_buffer.clear();
		});
	};

	/// Sets new rendering shader.
	void Layer::setShader(const sf::Shader* shader) {
		_buffer.states().shader = shader;
	};

	/// Configures intermediate rendering.
	void Layer::setArea(DimVector size, DimRect area) {
		_ir = ir_t(sf::RenderTexture(), size, area);
	};
	/// Removes an intermediate texture step.
	void Layer::removeArea() {
		_ir = {};
	};

	/// Returns layer rendering area.
	sf::IntRect Layer::view(sf::IntRect window) const {
		// pass through if no intermediate step
		if (!_ir) return window;

		// recalculate rendering area
		sf::Vector2i size = _ir->size.get(window.size, {});
		return _ir->area.get_es(window, size);
	};

	/// Maps a screen-space position into a layer position.
	sf::Vector2i Layer::map(sf::Vector2i pos, sf::IntRect window) const {
		// pass through if no intermediate step
		if (!_ir) return pos;

		// get texture configuration
		sf::Vector2i size = _ir->size.get(window.size, {});
		sf::IntRect area = _ir->area.get_es(window, size);

		// return remapped position
		sf::Vector2i npos = pos - area.position;
		return {
			(int)((float)npos.x * size.x / area.size.x),
			(int)((float)npos.y * size.y / area.size.y)
		};
	};

	/// Renders drawn layer onto render target.
	RenderStats Layer::render(sf::RenderTarget& target, sf::IntRect window) {
		// draw buffer to render target
		RenderStats stats;
		if (_ir) {
			// get rendering area
			sf::Vector2i size = _ir->size.get(window.size, {});
			sf::IntRect area = _ir->area.get_es(window, size);

			// reset intermediate texture
			auto _ = _ir->tex.resize((sf::Vector2u)size);
			_ir->tex.clear(sf::Color::Transparent);
			stats = _buffer.draw(_ir->tex);

			// configure intermediate sprite
			sf::Sprite spr(_ir->tex.getTexture());
			spr.setPosition((sf::Vector2f)area.position);
			spr.setScale({
				(float)area.size.x / size.x,
				(float)area.size.y / size.y
			});

			// render intermediate texture to target
			_ir->tex.display();
			target.draw(spr);
			stats.inters++;
		}
		else {
			// direct drawing
			stats = _buffer.draw(target);
		};
		return stats;
	};

	/// Creates a new interface layer.
	Layer* Interface::layer() {
		layers.push_back(std::unique_ptr<Layer>(new Layer));
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
		for (auto& layer : layers)
			layer->recalculate(delta, layer->view(winRect));
	};

	/// Send an event to interface.
	void Interface::event(const sf::Event& evt) {
		for (auto& layer : layers) {
			// check for keyboard events
			if (const auto* data = evt.getIf<sf::Event::KeyPressed>()) {
				layer->event((Event)Event::KeyPress {
					data->code, data->scancode, data->alt, data->shift, data->control, data->system
				});
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::KeyReleased>()) {
				layer->event((Event)Event::KeyRelease {
					data->code, data->scancode, data->alt, data->shift, data->control, data->system
				});
				return;
			};

			// check for mouse button events
			if (const auto* data = evt.getIf<sf::Event::MouseButtonPressed>()) {
				layer->event((Event)Event::MousePress {
					layer->map(data->position, winRect), data->button 
				});
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseButtonReleased>()) {
				layer->event((Event)Event::MouseRelease {
					layer->map(data->position, winRect), data->button
				});
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseMoved>()) {
				layer->event((Event)Event::MouseMove {
					layer->map(data->position, winRect)
				});
				return;
			};
			if (const auto* data = evt.getIf<sf::Event::MouseWheelScrolled>()) {
				layer->event((Event)Event::MouseWheel {
					layer->map(data->position, winRect), data->delta
				});
				return;
			};
		};
	};

	/// Sends an event queue to interface.
	void Interface::eventq(std::queue<sf::Event>& queue) {
		while (!queue.empty()) {
			event(queue.front());
			queue.pop();
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
			stats |= layer->render(target, winRect);
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
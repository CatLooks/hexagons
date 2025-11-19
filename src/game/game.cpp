#include "game/game.hpp"
#include "game/values/shared.hpp"
#include "mathext.hpp"

/// Constructs a game object.
Game::Game(ui::Layer* layer)
	: _layer(layer), _camera(layer, &map.camera, 17.f / 16)
{
	// setup camera
	_camera.minZoom = 0.5f;
	_camera.maxZoom = 2.0f;
	layer->infinite = true;

	// add tile selection handler
	layer->onEvent([=](const ui::Event& evt) {
		if (auto data = evt.get<ui::Event::MousePress>()) {
			if (data->button == sf::Mouse::Button::Left) {
				sf::Vector2i pos = mouseToHex(data->position);

				// tile selection logic
				return true;
			};
		};
		return false;
	});

	// add camera zoom handler
	layer->onEvent([=](const ui::Event& evt) {
		if (auto data = evt.get<ui::Event::MouseWheel>()) {
			_camera.scroll(-data->delta, ui::window.mouse(), ui::window.size());
			return true;
		};
		return false;
	});

	// add camera pan handler
	layer->onUpdate([=](const sf::Time& delta) {
		// set layer scale
		layer->setArea(ui::DimVector(1es, 1es) * _camera.zoom(), { 0px, 0px, 1ps, 1ps });

		// keyboard camera pan
		sf::Vector2i offset;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) offset.y--;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) offset.y++;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) offset.x--;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) offset.x++;
		map.camera += sf::Vector2i(sf::Vector2f(offset * Values::k)
			* Values::panSpeed * delta.asSeconds());

		// mouse camera pan
		_camera.pan(
			sf::Mouse::isButtonPressed(sf::Mouse::Button::Right),
			ui::window.mouse(), ui::window.size()
		);
	});
};

/// Returns hex coordinates at a mouse position.
sf::Vector2i Game::mouseToHex(sf::Vector2i mouse) const {
	// convert to world coordinates
	mouse += map.camera;

	// calculate y
	int y, ymod;
	ext::idivmod(mouse.y, Values::tileOff.y).into(y, ymod);

	// shift x if odd row
	mouse -= Values::rowOffset(y);

	// calculate x
	int x, xmod;
	ext::idivmod(mouse.x, Values::tileSize.x).into(x, xmod);

	// get base tile position
	sf::Vector2i pos = { x, y };

	// check for neighbor hops
	if (y < Values::tileSize.y - Values::tileOff.y) {
		// check neighboring hex
		if (xmod < Values::tileSize.x / 2) {
			// upper-left neighbor
			if (ymod < Values::tileUnit / 4 - xmod / 2)
				pos = map.neighbor(pos, Map::UpperLeft);
		} 
		else {
			// upper-right neighbor
			if (ymod < xmod / 2 - Values::tileUnit / 4)
				pos = map.neighbor(pos, Map::UpperRight);
		};
	};
	return pos;
};

/// Draws the map.
void Game::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
	map.draw(target);
};
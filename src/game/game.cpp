#include "game/game.hpp"
#include "game/values/shared.hpp"
#include "mathext.hpp"

/// Constructs a game object.
Game::Game(ui::Layer* layer, gameui::Panel* panel)
	: _layer(layer), _panel(panel), _camera(layer, &map.camera, 17.f / 16)
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
				Hex* hex = map.at(pos);

				// deselect
				if (!hex) {
					map.selected = Hex::Count;
					map._select = {};
					_panel->construct(gameui::Panel::None);
				}
				else if (hex->troop && hex->troop->type == Troop::Spearman) {
					map.selected = Hex::Count;
					map._select = pos;
					_panel->construct(gameui::Panel::L21);
					const auto& actions = _panel->actions();
					{
						_panel->preview()->addTexture(&assets::tilemap, Values::troop_textures[hex->troop->type]);
						auto text = _panel->preview()->addLabel();
						text->setPath("param");
						text->param("value", Values::troop_names[hex->troop->type]);
					}
					{
						actions[0]->addTexture(&assets::interface, Values::skills[static_cast<int>(SkillType::AttackSpear)]);
						auto text = actions[0]->addLabel();
						text->setRaw("Attack");
					}
					{
						actions[1]->addTexture(&assets::interface, Values::skills[static_cast<int>(SkillType::Shield)]);
						auto text = actions[1]->addLabel();
						text->setRaw("Defend");
					}
					{
						actions[2]->addTexture(&assets::interface, Values::skills[static_cast<int>(SkillType::Withdraw)]);
						auto text = actions[2]->addLabel();
						text->setRaw("Withdraw");
					}
				}
				else if (hex->build && hex->build->type == Build::Tower) {
					map.selected = Hex::Count;
					map._select = pos;
					_panel->construct(gameui::Panel::L01);
					const auto& actions = _panel->actions();
					{
						_panel->preview()->addTexture(&assets::tilemap, Values::build_textures[hex->build->type]);
						auto text = _panel->preview()->addLabel();
						text->setPath("param");
						text->param("value", Values::build_names[hex->build->type]);
					}
					{
						actions[0]->addTexture(&assets::interface, Values::skills[static_cast<int>(SkillType::Withdraw)]);
						auto text = actions[0]->addLabel();
						text->setRaw("Withdraw");
					}
				}
				else if (hex->plant) {
					map.selected = Hex::Count;
					map._select = pos;
					_panel->construct(gameui::Panel::L00);
					{
						_panel->preview()->addTexture(&assets::tilemap, Values::plant_textures[hex->plant->type]);
						auto text = _panel->preview()->addLabel();
						text->setPath("param");
						text->param("value", Values::plant_names[hex->plant->type]);
					}
				}
				else {
					map.selected = hex->team;
					map._select = {};
					_panel->construct(gameui::Panel::L22);
					const auto& actions = _panel->actions();
					{
						_panel->preview()->addDraw(gameui::preview::draw);
						
						gameui::preview::hex.team = hex->team;

						auto text = _panel->preview()->addLabel();
						text->setPath("gp.team");
						text->param("team", Values::hex_names[hex->team]);
						text->setColor(Values::hex_colors[hex->team]);
					}
					{
						actions[0]->addTexture(&assets::interface, Values::buy_build);
						actions[0]->addLabel()->setPath("gp.buy_build");
					}
					{
						actions[1]->addTexture(&assets::interface, Values::buy_troop);
						actions[1]->addLabel()->setPath("gp.buy_troop");
					}
					{
						actions[2]->addTexture(&assets::tilemap, Values::build_textures[Build::Farm]);
						actions[2]->addLabel()->setPath("build.farm");
						auto text = actions[2]->addSubtitle();
						text->setPath("gp.cost");
						text->param("cost", "4");
						text->setColor(sf::Color::Green);
					}
					{
						actions[3]->addTexture(&assets::tilemap, Values::troop_textures[Troop::Archer]);
						actions[3]->addLabel()->setPath("troop.archer");
						auto text = actions[3]->addSubtitle();
						text->setPath("gp.cost");
						text->param("cost", "12");
						text->setColor(sf::Color::Red);
					}
				};
				_panel->recalculate();
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
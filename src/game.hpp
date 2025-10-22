#pragma once

// include dependencies
#include "map.hpp"

// game ui element
class GameMap : public ui::Element {
public:
	Map map;                            // game map
	size_t turn = 1;                    // turn number
	sf::Vector2i selected = { -1, -1 }; // selected hex
	sf::Vector2i mouse;                 // mouse position

	/// Constructor.
	GameMap() {
		bounds = ui::DimRect::Fill;

		// create map
		map.resize({ 16, 10 },
			"      -        x"
			"   --     ------"
			"rr---   -----  x"
			"rrr---  ----    "
			"rr---   ----bbbx"
			"rrr---  -- -bbbb"
			"rr --- --  bbbbx"
			"    ------  bbb "
			"    ----    b  x"
			" -   ---   bb   "
		);

		// set map troops
		map.troop({ 1, 3 }, Troop::Worried);
		map.troop({ 14, 6 }, Troop::Worried);
		map.troop({ 12, 5 }, Troop::Evil);

		// camera move
		onUpdate([=](const sf::Time& delta) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
				map.camera.y += (int)(delta.asSeconds() * 5 * TILE);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
				map.camera.y -= (int)(delta.asSeconds() * 5 * TILE);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
				map.camera.x += (int)(delta.asSeconds() * 5 * TILE);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
				map.camera.x -= (int)(delta.asSeconds() * 5 * TILE);
		});

		// hex selector test
		onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::MousePress>()) {
				// get hex coords
				sf::Vector2i coords = map.mouseToHex(data->position);

				// ignore outside coordinates
				if (!map.within(coords)) {
					map.deselect();
					selected.x = -1;
					return true;
				};
				auto& hex = map.at(coords);

				// if already selected
				if (selected.x != -1) {
					// check for cancelation
					// check if move is illegal
					if (selected == coords || !hex.selected) {
						map.deselect();
						selected.x = -1;
						return true;
					};

					// move troop
					auto& old = map.at(selected);
					if (old.troop != ~0ull) {
						map.moveTroop(old.troop, coords);
						map.captureHex(hex, old.team);
					};
					map.troops[hex.troop].moved = true;

					// deselect
					map.deselect();
					selected.x = -1;
					return true;
				};

				// check if can select
				if (hex.team == Hex::Red && hex.troop != ~0ull && !map.troops[hex.troop].moved) {
					selected = coords;

					// select valid tiles
					Hex::Team team = hex.team;
					Troop::Type troop = map.troops[hex.troop].type;
					map.spread(coords,
						[&](Hex& hex, sf::Vector2i _) {
							hex.selected = true;
						},
						2,
						[&](const Hex& _, sf::Vector2i pos) -> bool {
							// block if not a land hex
							if (_.type == Hex::Void) return false;

							// check if any neighbors are the same team
							if (_.team != team) {
								for (size_t i = 0; i < 6; i++) {
									sf::Vector2i npos = hexNeighbor(pos, i);
									if (!map.within(npos)) continue;
									if (map.at(npos).team == team)
										return true;
								};
								return false;
							};
							return true;
						},
						[&](const Hex& hex, sf::Vector2i pos) -> bool {
							// ignore if no troop
							if (hex.troop == ~0ull) return true;

							// disable if ally troop
							if (hex.team == team) return false;

							// ignore if highest rank
							if (troop == Troop::Ultra) return true;

							// allow if equal or higher rank
							return troop >= map.troops[hex.troop].type;
						}
					);
				};
				return true;
			};
			return false;
		});

		// text settings
		ui::TextSettings sets = { assets::font, 24, assets::lang::locale };

		// economy text
		{
			ui::Text* text = new ui::Text(sets, "game.money");
			text->position() = {0.5as, 5px};
			text->autosize = true;
			text->setOutline(sf::Color::Black, 2.f);
			text->paramHook("money", [=]() -> ui::Text::Hook {
				return std::format("{}", map.econs[Hex::Red].balance);
			});
			add(text);
		}
		{
			ui::Text* text = new ui::Text(sets, "game.income");
			text->position() = { 0.5as, 35px };
			text->autosize = true;
			text->setOutline(sf::Color::Black, 2.f);
			text->paramHook("income", [=]() -> ui::Text::Hook {
				return std::format("{}", map.econs[Hex::Red].income);
				});
			add(text);
		}

		// text labels
		{
			ui::Text* text = new ui::Text(sets, "general.turns");
			text->position() = { 10px, 1ps - 36px };
			text->setOutline(sf::Color::Black, 2.f);
			text->paramHook("turn", [=]() -> ui::Text::Hook {
				return std::format("{}", turn);
			});
			add(text);
		}
		{
			ui::Text* text = new ui::Text(sets, "general.mouse");
			text->position() = { 10px, 1ps - 66px };
			text->setOutline(sf::Color::Black, 2.f);
			text->paramHook("x", [=]() -> ui::Text::Hook {
				sf::Vector2i coords = map.mouseToHex(this->mouse);
				return std::format("{}", coords.x);
			});
			text->paramHook("y", [=]() -> ui::Text::Hook {
				sf::Vector2i coords = map.mouseToHex(this->mouse);
				return std::format("{}", coords.y);
			});
			add(text);
		}

		// next turn button
		{
			ui::Image* img = new ui::Image({ { 192, 0 }, { 64, 64 } });
			img->margin.set(16);
			img->size() *= 2;
			img->position() = { 1as, 1as };
			add(img);

			img->onEvent([=](const ui::Event& evt) {
				if (auto data = evt.get<ui::Event::MousePress>()) {
					map.turn();
					turn++;
					return true;
				};
				return false;
			});
		}
	};

protected:
	// draws the game
	void drawSelf(ui::RenderBuffer& buffer, sf::IntRect self) const override {
		map.draw(buffer, selected);
	};
};
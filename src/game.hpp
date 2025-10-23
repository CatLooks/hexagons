#pragma once

// include dependencies
#include "map.hpp"

const int WIN_PERCENT = 75;

// random chance
// returns `true` with a `per` chance
bool chance(float per) {
	return rand() / (float)RAND_MAX < per;
};

// game ui element
class GameMap : public ui::Element {
public:
	Map map;                            // game map
	size_t turn = 1;                    // turn number
	sf::Vector2i selected = { -1, -1 }; // selected hex
	sf::Vector2i mouse;                 // mouse position
	ui::Interface* itf;
	bool focused;

	// selects all possible movements
	std::vector<sf::Vector2i> select(
		sf::Vector2i coords,
		std::function<void(Hex&, sf::Vector2i)> selector
	) {
		const Hex& origin = map.at(coords);
		Hex::Team team = origin.team;
		Troop::Type troop = map.troops[origin.troop].type;

		return map.spread(coords,
			selector, 2,
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
				// town hall check
				if (troop == Troop::Castle) {
					// check if tile is empty
					return hex.troop == ~0ull && hex.team == team;
				};

				// ignore if no troop
				if (hex.troop == ~0ull) return true;

				// merge if ally troop
				if (hex.team == team) {
					return troop == Troop::Worried
						&& map.troops[hex.troop].type == Troop::Worried;
				};

				// ignore if highest rank
				if (troop == Troop::Ultra) return true;

				// allow if higher rank
				return troop > map.troops[hex.troop].type;
			}
		);
	};

	/// Constructor.
	GameMap(ui::Interface* itf): itf(itf) {
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

		// create townhalls
		map.troop({ 1, 5 }, Troop::Castle);
		map.troop({ 14, 5 }, Troop::Castle);

		// set map troops
		map.troop({ 1, 3 }, Troop::Worried);
		map.troop({ 1, 4 }, Troop::Worried);
		map.troop({ 14, 6 }, Troop::Worried);
		map.troop({ 12, 5 }, Troop::Worried);

		// camera move
		onUpdate([=](const sf::Time& delta) {
			if (!focused) return;

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
					// auto& old = map.at(selected);
					// if (old.troop != ~0ull) {
					// 	map.moveTroop(old.troop, coords);
					// 	map.captureHex(hex, old.team);
					// };
					// map.troops[hex.troop].moved = true;
					auto& old = map.at(selected);
					if (old.troop != ~0ull)
						map.act(old.troop, selected, coords);

					// deselect
					map.deselect();
					selected.x = -1;
					return true;
				};

				// check if can select
				if (hex.team == Hex::Red && hex.troop != ~0ull && !map.troops[hex.troop].moved) {
					selected = coords;

					// don't select if not enough money
					if (!(map.troops[hex.troop].type == Troop::Castle && map.econs[hex.team].balance < 6))
						select(coords, [&](Hex& hex, sf::Vector2i pos) { hex.selected = true; });
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
			text->position() = { 0.5as, 5px };
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

		// ai economy text
		{
			ui::Text* text = new ui::Text(sets, "game.money");
			text->position() = { 1ps - 150px, 5px};
			text->autosize = true;
			text->setOutline(sf::Color::Black, 2.f);
			text->setColor(sf::Color(128, 128, 128));
			text->paramHook("money", [=]() -> ui::Text::Hook {
				return std::format("{}", map.econs[Hex::Blue].balance);
			});
			add(text);
		}
		{
			ui::Text* text = new ui::Text(sets, "game.income");
			text->position() = { 1ps - 150px, 35px };
			text->autosize = true;
			text->setOutline(sf::Color::Black, 2.f);
			text->setColor(sf::Color(128, 128, 128));
			text->paramHook("income", [=]() -> ui::Text::Hook {
				return std::format("{}", map.econs[Hex::Blue].income);
				});
			add(text);
		}

		// capture percentage text
		{
			ui::Text* text = new ui::Text(sets, "game.capture");
			text->position() = { 10px, 4px };
			text->setOutline(sf::Color::Black, 2.f);
			text->setColor(sf::Color(255, 0, 0));
			text->param("team", "Red");
			text->paramHook("per", [=]() -> ui::Text::Hook {
				return std::format("{}", 100 * map.econs[Hex::Red].tiles / map.tiles);
			});
			add(text);
		}
		{
			ui::Text* text = new ui::Text(sets, "game.capture");
			text->position() = { 10px, 34px };
			text->setOutline(sf::Color::Black, 2.f);
			text->setColor(sf::Color(0, 128, 255));
			text->param("team", "Blue");
			text->paramHook("per", [=]() -> ui::Text::Hook {
				return std::format("{}", 100 * map.econs[Hex::Blue].tiles / map.tiles);
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
					turn++;

					// ai moves
					for (size_t i = 0; i < map.troops.width(); i++) {
						// ignore if inactive
						if (!map.troops.active(i)) continue;
						Troop& troop = map.troops[i];
						auto& econ = map.econs[map.at(troop.pos).team];

						// ignore if not blue
						if (map.at(troop.pos).team != Hex::Blue) continue;

						// ignore if already moved
						if (troop.moved) continue;

						// get available moves
						auto moves = select(troop.pos, Map::noEffect);
						
						// ignore turn by chance
						if (chance(0.2)) continue;

						// town hall check
						if (troop.type == Troop::Castle) {
							// skip if no money
							if (econ.balance < 15)
								continue;

							// skip move if not enough income
							if (econ.income < 3 * Troop(Troop::Worried).cost())
								continue;

							// ignore by chance (x2)
							if (chance(0.4 + econ.balance / 400.f)) continue;
						};

						// split moves
						std::vector<sf::Vector2i> moves_exp;
						std::vector<sf::Vector2i> moves_inn;
						std::vector<sf::Vector2i> moves_mrg;

						for (sf::Vector2i move : moves) {
							if (map.at(move).team == Hex::None)
								moves_exp.push_back(move);
							else if (map.at(move).team == Hex::Blue && map.at(move).troop != ~0ull)
								moves_mrg.push_back(move);
							else
								moves_inn.push_back(move);
						};

						// merge check
						if (chance(0.3 + econ.balance / 300.f) && !moves_mrg.empty()
							&& econ.income >= Troop(Troop::Evil).cost() * 3 / 2)
						{
							size_t idx = rand() % moves_mrg.size();
							map.act(i, troop.pos, moves_mrg[idx]);
							troop.moved = true;
							continue;
						};

						// expansion check
						if (chance(
							econ.income == 0
								? 1.0
								: 0.2 + 4.0 / econ.income
						) && !moves_exp.empty())
						{
							size_t idx = rand() % moves_exp.size();
							map.act(i, troop.pos, moves_exp[idx]);
							troop.moved = true;
							continue;
						} 

						// do a random move
						if (!moves_inn.empty()) {
							size_t idx = rand() % moves_inn.size();
							map.act(i, troop.pos, moves_inn[idx]);
							troop.moved = true;
							continue;
						};
					};

					map.turn();
					finish();
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

	// tries to end the game
	void finish() {
		int a = 100 * map.econs[Hex::Red].tiles / map.tiles;
		int b = 100 * map.econs[Hex::Blue].tiles / map.tiles;

		bool bankrupcy = map.econs[Hex::Red].balance < 0 || map.econs[Hex::Blue].balance < 0;

		if (a >= WIN_PERCENT || b >= WIN_PERCENT || bankrupcy) {
			bool check = a >= WIN_PERCENT || map.econs[Hex::Blue].balance < 0;

			ui::Layer* layer = itf->layer(nullptr);

			ui::Solid* panel = new ui::Solid;
			panel->bounds = { 0.5as, 0.5as, 300px, 200px };
			panel->color = sf::Color::Black;
			panel->forwarding = true;
			layer->add(panel);

			// text settings
			ui::TextSettings sets = { assets::font, 24, assets::lang::locale };

			{
				ui::Text* title = new ui::Text(sets, (check ? "game.win" : "game.lose"));
				title->setOutline(sf::Color::Black, 4);
				title->setColor(check ? sf::Color::Green : sf::Color::Red);
				title->position() = { 0.5as, 0.5ps - 20px };
				title->shrink_to_fit = true;
				title->alignX = ui::Text::Center;
				title->alignY = ui::Text::Center;
				panel->add(title);
			}
			{
				ui::Text* data = new ui::Text(sets, bankrupcy ? "game.bankrupcy" : "game.stats");
				data->setOutline(sf::Color::Black, 4);
				data->position() = { 0.5as, 0.5ps + 20px };
				data->shrink_to_fit = true;
				data->alignX = ui::Text::Center;
				data->alignY = ui::Text::Center;
				panel->add(data);

				if (bankrupcy) {
					data->param("a", std::format("{}", map.econs[Hex::Red].balance));
					data->param("b", std::format("{}", map.econs[Hex::Blue].balance));
				} else {
					data->param("a", std::format("{}", a));
					data->param("b", std::format("{}", b));
				};
			}
		};
	};
};
#pragma once

// tile size
const int TILE = 96;

// include dependencies
#include <stdint.h>
#include <math.h>
#include <memory>
#include <list>
#include "troop.hpp"
#include "pool.hpp"
#include "hex.hpp"

// proper division oeration
int idiv(int a, int b) {
	return (int)floor((double)a / b);
};

// proper modulo operation
int imod(int a, int b) {
	int r = a % b;
	if (r < 0) r += b;
	return r;
};

// map object
struct Map {
	// array of hexes
	std::unique_ptr<Hex> data;

	// map size
	sf::Vector2i size;

	// troop pool
	Pool<Troop> troops;

	// camera position
	sf::Vector2i camera = { 32, 32 };

	// hex accessor
	Hex& at(sf::Vector2i pos) const {
		return data.get()[pos.y * size.x + pos.x];
	};

	// whether the position is within the world
	bool within(sf::Vector2i pos) const {
		return (pos.y >= 0 && pos.y < size.y)
			&& (pos.x >= 0 && pos.x < (size.x - !(pos.y & 1)));
	};

	// no effect function
	static void noEffect(Hex&, sf::Vector2i) {};
	// all hexes selector
	static bool allHexes(const Hex&, sf::Vector2i) { return true; };

	// spreads an effect on hexes in radius conditionally
	size_t spread(
		sf::Vector2i pos,
		std::function<void(Hex&, sf::Vector2i)> effect,
		int radius,
		std::function<bool(const Hex&, sf::Vector2i)> blockingCondition = allHexes,
		std::function<bool(const Hex&, sf::Vector2i)> nonBlockingCondition = allHexes
	) {
		// get new spread index
		static size_t next_idx = 1;
		size_t spr_idx = next_idx++;
		at(pos).buffer = spr_idx;

		// hex data in queue
		struct qd_t {
			sf::Vector2i pos; // hex position
			int left;        // spread depth left
		};

		// queued hexes
		std::list<qd_t> queued;
		queued.push_back({ pos, radius });

		// spread to queued hexes
		while (!queued.empty()) {
			// process a single hex
			auto data = queued.front();
			queued.pop_front();
			Hex& hex = at(data.pos);

			// mark hex as checked
			hex.buffer = spr_idx;

			// check if hex passes the condition
			if (!blockingCondition(hex, data.pos)) continue;

			// apply effect on hex
			if (nonBlockingCondition(hex, data.pos)) effect(hex, data.pos);

			// queue neighboring hexes
			if (data.left > 0) {
				for (size_t nbi = 0; nbi < 6; nbi++) {
					// ignore if processed
					sf::Vector2i npos = hexNeighbor(data.pos, nbi);
					if (!within(npos)) continue;
					if (at(npos).buffer == spr_idx) continue;

					// queue neighbor
					queued.push_back({ npos, data.left - 1 });
				};
			};
		};

		// return spread index
		return spr_idx;
	};

	// create a new map from a string
	void resize(sf::Vector2i size, const char* str) {
		this->size = size;
		data = std::unique_ptr<Hex>(new Hex[size.x * size.y]);
		size_t pos = 0;

		for (size_t i = 0; str[i]; i++) {
			data.get()[pos].type = str[i] == ' ' ? Hex::Void : Hex::Land;
			switch (str[i]) {
				case 'r': data.get()[pos].team = Hex::Red; break;
				case 'b': data.get()[pos].team = Hex::Blue; break;
				default: data.get()[pos].team = Hex::None; break;
			};
			pos++;
		};
	};

	// creates a new troop at position
	void troop(sf::Vector2i pos, Troop::Type type) {
		size_t idx = troops.push(Troop { type, pos });
		at(pos).troop = idx;
	};

	// moves a troop to a new position
	void moveTroop(size_t idx, sf::Vector2i pos) {
		Hex& old = at(troops[idx].pos);
		Hex& now = at(pos);

		// check if troop is getting replaced
		if (now.troop != ~0ull)
			troops.pop(now.troop);

		// move troop
		troops[idx].pos = pos;
		old.troop = ~0ull;
		now.troop = idx;
	};

	// converts mouse coords into world coords
	sf::Vector2i mouseToHex(sf::Vector2i mouse) const {
		// subtract base offset
		mouse -= camera;

		// get y
		int y = idiv(mouse.y, (TILE * 3 / 4));
		int ymod = imod(mouse.y, (TILE * 3 / 4));
		// equalize row x
		mouse.x -= !(y & 1) ? (TILE / 2) : 0;

		// check for solid tile part
		sf::Vector2i pos = { idiv(mouse.x, TILE), y };

		// reposition to neighbors
		if (y < TILE / 4) {
			// get x within the hex
			int xmod = imod(mouse.x, TILE);

			// check neighboring hex
			if (imod(mouse.x, TILE) < TILE / 2) {
				// neighboring nbi 5
				if (ymod < TILE / 4 - xmod / 2)
					pos = hexNeighbor(pos, 5);
			} else {
				// neighboring nbi 0
				if (ymod < xmod / 2 - TILE / 4)
					pos = hexNeighbor(pos, 0);
			};
		};
		return pos;
	};

	// draws the map
	void draw(ui::RenderBuffer& target, sf::Vector2i selection, size_t mask) const {
		for (int y = 0; y < size.y; y++) {
			// row offset
			bool shift = !(y & 1);
			sf::Vector2i row_off = sf::Vector2i(
				shift ? (TILE / 2) : 0,
				y * (TILE * 3 / 4)
			) + camera;

			for (int x = 0; x < size.x; x++) {
				// skip last hex at odd rows
				if (x + 1 == size.x && shift) continue;

				// ignore if void
				const Hex& hex = at({x, y});
				if (hex.type == Hex::Void) continue;
				
				// get hex tint
				sf::Color tint;
				switch (hex.team) {
					case Hex::Red: tint = sf::Color(255, 128, 128); break;
					case Hex::Blue: tint = sf::Color(128, 192, 255); break;
					default: tint = sf::Color(96, 96, 96); break;
				};

				// draw hex
				sf::Vector2i coords = row_off + sf::Vector2i(x * TILE, 0);
				target.quad(
					{ coords, { TILE, TILE } },
					{ { 0, 0 }, { 64, 64 } },
					tint
				);

				// draw selection
				if (selection == sf::Vector2i(x, y)) {
					target.quad(
						{ coords, { TILE, TILE } },
						{ { 128, 0 }, { 64, 64 } },
						sf::Color::White
					);
				};

				// draw troop
				if (hex.troop != ~0ull) {
					troops[hex.troop].draw(coords, target);
				};

				// draw mask
				if (selection.x != -1 && hex.buffer != mask) {
					target.quad(
						{ coords, { TILE, TILE } },
						{ { 192, 64 }, { 64, 64 } }
					);
				};
			};
		};
	};
};

// game ui element
class GameMap : public ui::Element {
public:
	Map map;                            // game map
	size_t turn = 1;                    // turn number
	sf::Vector2i selected = { -1, -1 }; // selected hex
	size_t mask = ~0ull;                // selection mask

	/// Constructor.
	GameMap() {
		bounds = ui::DimRect::Fill;

		// create map
		map.resize({16, 10},
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

		// hex selector test
		onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::MousePress>()) {
				// get hex coords
				sf::Vector2i coords = map.mouseToHex(data->position);

				// ignore outside coordinates
				if (!map.within(coords)) {
					selected.x = -1;
					return true;
				};
				auto& hex = map.at(coords);

				// if already selected
				if (selected.x != -1) {
					// check for cancelation
					// check if move is illegal
					if (selected == coords || hex.buffer != mask) {
						selected.x = -1;
						return true;
					};

					// move troop
					auto& old = map.at(selected);
					if (old.troop != ~0ull) {
						map.moveTroop(old.troop, coords);
						hex.team = old.team;
					};

					// deselect
					selected.x = -1;
					return true;
				};
				
				// check if can select
				if (hex.team == Hex::Red && hex.troop != ~0ull) {
					selected = coords;

					// select valid tiles
					Hex::Team team = hex.team;
					Troop::Type troop = map.troops[hex.troop].type;
					mask = map.spread(coords, Map::noEffect, 2,
						[&](const Hex& _, sf::Vector2i pos) -> bool {
							// block if not a land hex
							if (hex.type == Hex::Void) return false;

							// check if any neighbors are the same team
							if (_.team != team) {
								for (size_t i = 0; i < 6; i++) {
									sf::Vector2i npos = hexNeighbor(pos, i);
									if (!map.within(npos)) continue;
									if (map.at(npos).team == team) return true;
								};
								return false;
							};
							return true;
						},
						[&](const Hex& hex, sf::Vector2i pos) -> bool {
							// ignore if no troop
							if (hex.troop == ~0ull) return true;

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

		// text labels
		{
			ui::Text* text = new ui::Text(sets, "general.turns");
			text->position() = { 10px, 1ps - 36px };
			text->paramHook("turn", [=]() -> ui::Text::Hook {
				return std::format("{}", turn);
			});
			add(text);
		}
		{
			ui::Text* text = new ui::Text(sets, "general.mouse");
			text->position() = { 10px, 1ps - 66px };
			text->paramHook("x", [=]() -> ui::Text::Hook {
				sf::Vector2i coords = map.mouseToHex(sf::Mouse::getPosition(win));
				return std::format("{}", coords.x);
			});
			text->paramHook("y", [=]() -> ui::Text::Hook {
				sf::Vector2i coords = map.mouseToHex(sf::Mouse::getPosition(win));
				return std::format("{}", coords.y);
			});
			add(text);
		}
	};

protected:
	// draws the game
	void drawSelf(ui::RenderBuffer& buffer, sf::IntRect self) const override {
		map.draw(buffer, selected, mask);
	};
};
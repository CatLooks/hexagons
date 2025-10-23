#pragma once

// tile size
const int TILE = 128;

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

// economy object
struct Economy {
	int balance;
	int income;
	int tiles;
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

	// player economies
	Economy econs[3];

	// tile count
	size_t tiles = 0;

	// hex accessor
	Hex& at(sf::Vector2i pos) const {
		return data.get()[pos.y * size.x + pos.x];
	};

	// whether the position is within the world
	bool within(sf::Vector2i pos) const {
		return (pos.y >= 0 && pos.y < size.y)
			&& (pos.x >= 0 && pos.x < (size.x - (pos.y & 1 ? 0 : 1)));
	};

	// updates
	void captureHex(Hex& hex, Hex::Team team) {
		econs[hex.team].income--;
		econs[hex.team].tiles--;
		hex.team = team;
		econs[hex.team].income++;
		econs[hex.team].tiles++;
	};

	// no effect function
	static void noEffect(Hex&, sf::Vector2i) {};
	// all hexes selector
	static bool allHexes(const Hex&, sf::Vector2i) { return true; };

	// spreads an effect on hexes in radius conditionally
	std::vector<sf::Vector2i> spread(
		sf::Vector2i pos,
		std::function<void(Hex&, sf::Vector2i)> effect,
		int radius,
		std::function<bool(const Hex&, sf::Vector2i)> blockingCondition = allHexes,
		std::function<bool(const Hex&, sf::Vector2i)> nonBlockingCondition = allHexes
	) const {
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
		std::vector<sf::Vector2i> results;
		while (!queued.empty()) {
			// process a single hex
			qd_t data = queued.front();
			queued.pop_front();
			Hex& hex = at(data.pos);

			// mark hex as checked
			hex.buffer = spr_idx;

			// check if hex passes the condition
			if (!blockingCondition(hex, data.pos))
				continue;

			// apply effect on hex
			if (nonBlockingCondition(hex, data.pos)) {
				effect(hex, data.pos);
				results.push_back(data.pos);
			};

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

		// return affected hexes
		return results;
	};

	// create a new map from a string
	void resize(sf::Vector2i size, const char* str) {
		this->size = size;
		data = std::unique_ptr<Hex>(new Hex[size.x * size.y]);
		size_t pos = 0;

		// reset economies
		for (size_t i = 0; i < 3; i++) {
			econs[i] = { 10, 0, 0 };
		};
		tiles = 0;

		// create map
		for (size_t i = 0; str[i]; i++) {
			data.get()[pos].type = str[i] == ' ' ? Hex::Void : Hex::Land;
			if (data.get()[pos].type)
				tiles++;
			switch (str[i]) {
				case 'r': data.get()[pos].team = Hex::Red; break;
				case 'b': data.get()[pos].team = Hex::Blue; break;
				default: data.get()[pos].team = Hex::None; break;
			};
			econs[data.get()[pos].team].income++;
			econs[data.get()[pos].team].tiles++;
			pos++;
		};
	};

	// creates a new troop at position
	void troop(sf::Vector2i pos, Troop::Type type) {
		size_t idx = troops.push(Troop { type, pos });
		at(pos).troop = idx;
		econs[at(pos).team].income -= troops[idx].cost();
	};

	// moves a troop to a new position
	void moveTroop(size_t idx, sf::Vector2i pos) {
		Hex& old = at(troops[idx].pos);
		Hex& now = at(pos);

		// check if troop is getting replaced
		if (now.troop != ~0ull) {
			econs[now.team].income += troops[now.troop].cost();
			troops.pop(now.troop);
		};

		// move troop
		troops[idx].pos = pos;
		old.troop = ~0ull;
		now.troop = idx;
	};

	// moves a troop from an old to a new position
	void act(size_t troop, sf::Vector2i old, sf::Vector2i now) {
		Hex& hex = at(now);
		Hex& last = at(troops[troop].pos);

		// town hall spawn
		if (troops[troop].type == Troop::Castle) {
			this->troop(now, Troop::Worried);
			econs[at(troops[troop].pos).team].balance -= 6;
			return;
		};

		// troop move
		if (last.team == hex.team && hex.troop != ~0ull) {
			econs[hex.team].income += troops[troop].cost();

			troops[hex.troop].type = Troop::Evil;
			troops.pop(troop);
			last.troop = ~0ull;

			econs[hex.team].income -= troops[hex.troop].cost();
		} else {
			moveTroop(troop, now);
			captureHex(hex, last.team);
		};
		troops[troop].moved = true;
	};

	// deselects all hexes
	void deselect() const {
		for (size_t i = 0; i < size.x * size.y; i++)
			data.get()[i].selected = false;
	};

	// resets turn cooldowns
	void turn() {
		// reset troop cooldown
		for (size_t i = 0; i < troops.width(); i++) {
			if (troops.active(i)) {
				troops[i].moved = false;
			};
		};

		// update economies
		for (size_t i = 0; i < 3; i++) {
			econs[i].balance += econs[i].income;
		};
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
	void draw(ui::RenderBuffer& target, sf::Vector2i selection) const {
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
				if (selection.x != -1 && !hex.selected) {
					target.quad(
						{ coords, { TILE, TILE } },
						{ { 192, 64 }, { 64, 64 } }
					);
				};
			};
		};
	};
};
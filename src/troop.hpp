#pragma once

// include dependencies
#include <stdint.h>
#include <SFML/System.hpp>

// troop object
struct Troop {
	// troop type
	enum Type {
		Worried, // 😧 (but blue)
		Evil,    // 😡
		Ultra,   // 🗿
	} type;

	// coordinates
	sf::Vector2i pos;

	// whether the troop has already moved
	bool moved = false;

	// troop sustain cost
	int cost() const {
		switch (type) {
			case Worried: return 4;
			case Evil: return 12;
			default: return 0;
		};
	};

	// draws the troop
	void draw(sf::Vector2i coords, ui::RenderBuffer& target) const {
		// get troop texture
		sf::Vector2i texture;
		switch (type) {
			case Worried: texture = { 0, 64 }; break;
			case Evil: texture = { 64, 64 }; break;
			default: break;
		};

		// draw unmoved aura
		if (!moved) {
			target.quad(
				{ coords, { TILE, TILE } },
				{ { 128, 64 }, { 64, 64 } }
			);
		};

		// draw troop
		target.quad(
			{ coords, { TILE, TILE } },
			{ texture, { 64, 64 } }
		);
	};
};
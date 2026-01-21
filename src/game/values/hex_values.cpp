#include "game/values/hex_values.hpp"

namespace Values {
	/// Water hex.
	const sf::IntRect water = coords(8, 1);
	/// Hex sides.
	const sf::IntRect sides = coords(8, 3);
	const sf::IntRect sideShade = coords(8, 2);
	/// Hex mask.
	const sf::IntRect mask = coords(9, 0);
	/// Hex pulse.
	const sf::IntRect pulse = {
		{ 6 * Values::tileTex.x, 1 * Values::tileTex.y },
		Values::tileTex + Values::tileLevel(Values::tileTex)
	};
	/// Defense shield.
	const sf::IntRect shield = coords(9, 1);

	/// Water side color.
	const sf::Color waterSide = sf::Color(0, 157, 251);

	// texture array
	const sf::IntRect hex_textures[Region::Count] = {
		coords(0, 0), // void
		coords(1, 0), // red
		coords(2, 0), // orange
		coords(3, 0), // yellow
		coords(4, 0), // green
		coords(5, 0), // aqua
		coords(6, 0), // blue
		coords(7, 0), // purple
		coords(8, 0), // magenta
	};

	// name array
	const char* hex_names[Region::Count] = {
		"@!team.v", "@!team.r", "@!team.o",
		"@!team.y", "@!team.g", "@!team.a",
		"@!team.b", "@!team.p", "@!team.m",
	};

	// color array
	const sf::Color hex_colors[Region::Count] = {
		sf::Color(0x80, 0x80, 0x80),
		sf::Color(0xff, 0x20, 0x40),
		sf::Color(0xff, 0x88, 0x40),
		sf::Color(0xff, 0xf0, 0x40),
		sf::Color(0x40, 0xff, 0x20),
		sf::Color(0x40, 0xf0, 0xff),
		sf::Color(0x20, 0x60, 0xff),
		sf::Color(0x90, 0x60, 0xff),
		sf::Color(0xff, 0x60, 0xff)
	};
};
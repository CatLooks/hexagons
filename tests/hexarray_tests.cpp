#include "game/array.hpp"
#include <cassert>

int main() {
	// neighbor offset dla (2,2) i wszystkich 6 kierunków
	const sf::Vector2i c{2, 2};
	assert(HexArray::neighbor(c, HexArray::UpperRight) == sf::Vector2i(3, 1));
	assert(HexArray::neighbor(c, HexArray::Right)      == sf::Vector2i(3, 2));
	assert(HexArray::neighbor(c, HexArray::LowerRight) == sf::Vector2i(3, 3));
	assert(HexArray::neighbor(c, HexArray::LowerLeft)  == sf::Vector2i(2, 3));
	assert(HexArray::neighbor(c, HexArray::Left)       == sf::Vector2i(1, 2));
	assert(HexArray::neighbor(c, HexArray::UpperLeft)  == sf::Vector2i(2, 1));

	// distance – proste przypadki
	assert(HexArray::distance({0, 0}, {0, 0}) == 0);
	assert(HexArray::distance({0, 0}, {1, 0}) == 1);
	assert(HexArray::distance({0, 0}, {2, 2}) >= 2); // siatka skoœna, ale odleg³oœæ > 1

	return 0;
}
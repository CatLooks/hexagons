#include "game/spread.hpp"
#include "game/array.hpp"
#include <cassert>

int main() {
	HexArray arr;
	arr.empty({ 4, 4 });

	// Ustaw wszystkie hexy jako Ground, ¿eby by³y "solid"
	for (int y = 0; y < arr.size().y; ++y) {
		for (int x = 0; x < arr.size().x; ++x) {
			if (auto* h = arr.at({ x, y })) h->type = Hex::Ground;
		}
	}

	Spread sp;
	int hits = 0;
	sp.effect = [&](const Spread::Tile&) { hits++; };

	auto visited = sp.applylist(arr, { 1, 1 }, 1);

	assert(hits == static_cast<int>(visited.size()));
	assert(visited.size() == 6);               // œrodkowy hex ma 6 s¹siadów
	for (auto p : visited) {
		assert(!(p == sf::Vector2i{ 1, 1 })); // origin nie powinien byæ w visited
		assert(HexArray::distance(p, { 1, 1 }) == 1);
	}
	return 0;
}
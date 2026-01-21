#include "game/array.hpp"
#include "random.hpp"
#include <cassert>

static sf::Vector2i random_inside(const HexArray& arr) {
	while (true) {
		int y = (int)(Random::u32() % arr.size().y);
		int x = (int)(Random::u32() % arr.size().x);
		sf::Vector2i p{ x, y };
		if (arr.contains(p)) return p;
	}
}

int main() {
	Random::seed(42);
	HexArray arr;
	arr.empty({ 32, 32 });

	for (int i = 0; i < 5000; ++i) {
		sf::Vector2i p = random_inside(arr);

		for (int n = 0; n < 6; ++n) {
			auto q = arr.neighbor(p, static_cast<HexArray::nbi_t>(n));
			if (arr.contains(q))
				assert(HexArray::distance(p, q) == 1);
		}

		sf::Vector2i b = random_inside(arr);
		assert(HexArray::distance(p, b) == HexArray::distance(b, p));
	}
	return 0;
}
#include "ui/units.hpp"
#include <chrono>
#include <cstdio>

int main() {
	const int iterations = 2'000'000;

	ui::DimRect rect(
		10px, 5px,
		0.5ps + 3ts, 0.25ps + 1ts
	);

	sf::IntRect parent({0, 0}, {1920, 1080});
	sf::Vector2i es = {256, 128};

	auto start = std::chrono::steady_clock::now();
	sf::IntRect last{};
	for (int i = 0; i < iterations; ++i) {
		last = rect.get_es(parent, es);
		rect.position.x += 0.001f; // lekka zmiana, ¿eby nie zoptymalizowa³o do sta³ej
	}
	auto end = std::chrono::steady_clock::now();

	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	if (last.size.x == 0 || last.size.y == 0) return 1;

	std::printf("perf_dim: %d iters in %lld ms (last %d x %d)\n",
		iterations, static_cast<long long>(ms), last.size.x, last.size.y);
	return 0;
}
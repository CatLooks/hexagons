#include "mathext.hpp"
#include "random.hpp"
#include <cassert>
#include <cmath>

int main() {
	Random::seed(123);

	// spójnoœæ idiv/imod/idivmod
	for (int i = 0; i < 20000; ++i) {
		int a = (int)(Random::u32() % 2000) - 1000;
		int b = (int)(Random::u32() % 99) - 49;
		if (b == 0) b = 1;

		auto dm = ext::idivmod(a, b);
		assert(dm.first == ext::idiv(a, b));
		assert(dm.second == ext::imod(a, b));
	}

	// podstawowe w³asnoœci fpown na bezpiecznych wartoœciach
	for (int i = 1; i <= 10; ++i) {
		float x = 0.2f * (float)i; // dodatnie, unikamy zera
		assert(std::fabs(ext::fpown(x, 0) - 1.0f) < 1e-6f);
		assert(std::fabs(ext::fpown(x, 1) - x) < 1e-6f);
		float inv = ext::fpown(x, -1);
		assert(std::isfinite(inv));
		assert(std::fabs(inv * x - 1.0f) < 1e-3f);
		for (int p = 2; p <= 6; ++p) {
			float fwd = ext::fpown(x, p);
			float bwd = ext::fpown(x, -p);
			assert(std::isfinite(fwd) && std::isfinite(bwd));
			assert(std::fabs(fwd * bwd - 1.0f) < 1e-2f);
		}
	}
	return 0;
}
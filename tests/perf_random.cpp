#include "random.hpp"
#include <chrono>
#include <cstdint>
#include <cstdio>

int main() {
	const int iterations = 5'000'000;
	Random::seed(1234);

	auto start = std::chrono::steady_clock::now();
	uint32_t acc = 0;
	for (int i = 0; i < iterations; ++i)
		acc ^= Random::u32();
	auto end = std::chrono::steady_clock::now();

	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	// prosta sanity-check: wynik powinien byæ niezerowy
	if (acc == 0) return 1;

	std::printf("perf_random: %d iters in %lld ms\n", iterations, static_cast<long long>(ms));
	return 0;
}
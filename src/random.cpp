#include "random.hpp"
#include <stdlib.h>

namespace Random {
	/// Reset generator and sets a seed.
	void seed(size_t value) {
		srand(value);
	};

	/// Returns a random unsigned 8-bit number.
	uint8_t u8() { return rand() & 0xff; };
	/// Returns a random unsigned 16-bit number.
	uint16_t u16() { return u8() | (uint16_t)u8() << 8; };
	/// Returns a random unsigned 32-bit number.
	uint32_t u32() { return u16() | (uint32_t)u16() << 16; };
	/// Returns a random unsigned 64-bit number.
	uint64_t u64() { return u32() | (uint64_t)u32() << 32; };

	/// Returns a random float in range `[0, 1)`.
	float uniform() { return (float)u32() / UINT32_MAX; };

	/// Returns whether a chance succeeds.
	bool chance(float value) { return uniform() < value; };

	/// Rolls a chance with pity.
	bool pity(float value, uint8_t& fail, uint8_t max) {
		// bonus chance per fail
		float slope = (1.f - value) / max;

		// roll a chance with pity
		bool roll = chance(value + slope * fail);

		// reset or increase fail counter
		fail = roll ? 0 : fail + 1;
		return roll;
	};
};
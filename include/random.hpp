#pragma once

// include dependencies
#include <stdint.h>

/// Randomness generator namespace.
namespace Random {
	/// Reset generator and sets a seed.
	///
	/// @param value Seed number.
	void seed(size_t value);

	/// Returns a random unsigned 8-bit number.
	uint8_t u8();
	/// Returns a random unsigned 16-bit number.
	uint16_t u16();
	/// Returns a random unsigned 32-bit number.
	uint32_t u32();
	/// Returns a random unsigned 64-bit number.
	uint64_t u64();

	/// Returns a random float in range `[0, 1)`.
	float uniform();

	/// Returns whether a chance succeeds.
	///
	/// @param value Success rate.
	bool chance(float value);

	/// Rolls a chance with pity.
	///
	/// @param value Base roll chance.
	/// @param fail Fail counter.
	/// @param max Maximum amount of fails.
	bool pity(float value, uint8_t& fail, uint8_t max);
};
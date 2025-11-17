#include "mathext.hpp"

namespace ext {
	/// Returns `a / d` rounded down.
	int idiv(int a, int d) {
		return (int)floorf((float)a / d);
	};

	/// Returns `a % b` rounded down.
	int imod(int a, int d) {
		int r = a % d;
		if (r < 0) r += d;
		return r;
	};

	/// Calculates quotient and remainder rounded down.
	void idivmod(int a, int d, int& q, int& r) {
		q = idiv(a, d);
		r = imod(a, d);
	};

	/// Returns smaller integer.
	int imin(int a, int b) {
		return a < b ? a : b;
	};
	/// Returns greater integer.
	int imax(int a, int b) {
		return a > b ? a : b;
	};
	/// Clamps an integer to an inclusive range.
	int iclamp(int x, int lower, int upper) {
		return imin(imax(x, lower), upper);
	};

	/// Returns smaller float.
	float fmin(float a, float b) {
		return a < b ? a : b;
	};
	/// Returns greater float.
	float fmax(float a, float b) {
		return a > b ? a : b;
	};
	/// Clamps a float to an inclusive range.
	float fclamp(float x, float lower, float upper) {
		return fmin(fmax(x, lower), upper);
	};
};

/// Shifts a rectangle by a vector.
sf::IntRect operator+(const sf::IntRect& rect, const sf::Vector2i& shift) {
	return { rect.position + shift, rect.size };
};
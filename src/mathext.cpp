#include "mathext.hpp"
#include <sstream>
#include <iomanip>

namespace ext {
	/// Returns absolute value of an integer.
	int iabs(int x) {
		return x >= 0 ? x : -x;
	};

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
	Pair<int, int> idivmod(int a, int d) {
		return { idiv(a, d), imod(a, d) };
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

	// power for unsigned integer powers
	static float _pow(float x, int p) {
		float a = 1.f;
		while (p > 0) {
			if (p & 1) a *= x;
			x *= x; p >>= 1;
		};
		return a;
	};

	/// Raises a float to an integer power.
	float fpown(float base, int index) {
		return index >= 0
			? _pow(base, index)
			: 1.f / _pow(base, -index);
	};

	/// Returns a string representation of a vector.
	std::string str_vec(sf::Vector2i vec) {
		return std::format("{}, {}", vec.x, vec.y);
	};

	/// Returns a string representation of a rectangle.
	std::string str_rect(sf::IntRect rect) {
		return std::format(
			"{}, {} ({} x {})",
			rect.position.x, rect.position.y,
			rect.size.x, rect.size.y
		);
	};

	/// Returns a string representation of a signed integer.
	std::string str_int(int n) {
		return std::format("{}", n);
	};

	/// Returns a string representation of an unsigned integer.
	std::string str_int(size_t n) {
		return std::format("{}", n);
	};

	/// Returns a string representation of a float.
	std::string str_float(float f, unsigned int digits) {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(digits) << f;
		return ss.str();
	};

	/// Returns percentage of `count` in `total`.
	std::string str_percent(size_t count, size_t total) {
		if (!count && !total) return "0%";
		return std::format("{:.1f}%", (float)count / total * 100.f);
	};
};

/// Shifts a rectangle by a vector.
sf::IntRect operator+(const sf::IntRect& rect, const sf::Vector2i& shift) {
	return { rect.position + shift, rect.size };
};

/// Shifts a rectangle by an inverted vector.
sf::IntRect operator-(const sf::IntRect& rect, const sf::Vector2i& shift) {
	return { rect.position - shift, rect.size };
};
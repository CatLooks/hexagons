#pragma once

// include dependencies
#define _USE_MATH_DEFINES
#include "math.h"
#include <SFML/Graphics/Rect.hpp>

/// Math extension function namespace.
namespace ext {
	/// Pair of 2 values.
	///
	/// @tparam Type of first object.
	/// @tparam Type of second object.
	template <typename A, typename B> struct Pair {
		A a;
		B b;

		/// Constructs a pair.
		///
		/// @param a First value.
		/// @param b Second value.
		Pair(A a, B b): a(a), b(b) {};

		/// Copies values from the pair into references.
		///
		/// @param reference to first object destination.
		/// @param reference to second object destination.
		void into(A& a, B& b) {
			a = this->a;
			b = this->b;
		};
	};

	/// Returns `a / d` rounded down.
	///
	/// @param a Divident.
	/// @param d Divisor.
	///
	/// @return Proper quotient.
	int idiv(int a, int d);

	/// Returns `a % b` rounded down.
	///
	/// @param a Divident.
	/// @param d Divisor.
	///
	/// @return Proper remainder ("modulo").
	int imod(int a, int d);

	/// Calculates quotient and remainder rounded down.
	///
	/// @param a Divident.
	/// @param d Divisor.
	///
	/// @return Quotient and remainder pair.
	Pair<int, int> idivmod(int a, int d);

	/// Returns smaller integer.
	int imin(int a, int b);
	/// Returns greater integer.
	int imax(int a, int b);
	/// Clamps an integer to an inclusive range.
	///
	/// @param x Clamped value.
	/// @param lower Lower bound.
	/// @param upper Upper bound.
	int iclamp(int x, int lower, int upper);

	/// Returns smaller float.
	float fmin(float a, float b);
	/// Returns greater float.
	float fmax(float a, float b);
	/// Clamps a float to an inclusive range.
	///
	/// @param x Clamped value.
	/// @param lower Lower bound.
	/// @param upper Upper bound.
	float fclamp(float x, float lower, float upper);

	/// Raises a float to an integer power.
	/// 
	/// @param base Power base.
	/// @param index Power index.
	/// 
	/// @return `base ^ index`.
	float fpown(float base, int index);
};

/// Shifts a rectangle by a vector.
///
/// @param rect Original rectangle.
/// @param shift Shift vector.
///
/// @return Shifted rectangle.
sf::IntRect operator+(const sf::IntRect& rect, const sf::Vector2i& shift);

/// Shifts a rectangle by an inverted vector.
///
/// @param rect Original rectangle.
/// @param shift Shift vector.
///
/// @return Shifted rectangle.
sf::IntRect operator-(const sf::IntRect& rect, const sf::Vector2i& shift);
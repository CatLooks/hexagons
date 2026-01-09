#pragma once

// include dependencies
#define _USE_MATH_DEFINES
#include "math.h"
#include <SFML/Graphics/Rect.hpp>
#include <format>

/// Math extension function namespace.
namespace ext {
	/// Returns absolute value of an integer.
	/// 
	/// @param x Any integer.
	/// 
	/// @return Non-negative integer.
	int iabs(int x);

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
	std::pair<int, int> idivmod(int a, int d);

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

	/// Returns a string representation of a vector.
	/// 
	/// @param vec Source vector.
	/// @param sep Separator.
	std::string str_vec(sf::Vector2i vec, const char* sep = ", ");

	/// Returns a string representation of a rectangle.
	/// 
	/// @param vec Source rectangle.
	std::string str_rect(sf::IntRect rect);

	/// Returns a string representation of a signed integer.
	/// 
	/// @param vec Source integer.
	std::string str_int(int n);

	/// Returns a string representation of an unsigned integer.
	/// 
	/// @param vec Source integer.
	std::string str_int(size_t n);

	/// Returns a string representation of a float.
	/// 
	/// @param f Source float.
	/// @param digits Digits after floating point.
	std::string str_float(float f, unsigned int digits = 1);

	/// Returns percentage of `count` in `total`.
	/// 
	/// @param count Partial amount.
	/// @param total Total amount.
	/// 
	/// @return Percentage string.
	std::string str_percent(size_t count, size_t total);

	/// Returns a string representation of a time.
	///
	/// @param time Time in seconds.
	std::string str_time(float time);
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
#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>

namespace ui {

	/// Linearly interpolates between 2 integers.
	/// 
	/// @param a Starting integer.
	/// @param b Ending integer.
	/// @param t Interpolation progress.
	/// 
	/// @return Interpolated integer.
	int lerpi(int a, int b, float t) { return a + (int)((b - a) * t); };

	/// Linearly interpolates between 2 floats.
	/// 
	/// @param a Starting float.
	/// @param b Ending float.
	/// @param t Interpolation progress.
	/// 
	/// @return Interpolated float.
	float lerpf(float a, float b, float t) { return a + (b - a) * t; };

	/// Axis dimension.
	/// - Allows to describe position relative to element and its parent size.
	/// - Allows to describe size relative to element size.
	struct Dim {
		int   px; /// Base value (in pixels).
		float ps; /// Parent size scalar.
		float es; /// Element size scalar.

		/// Converts dimension into its true position.
		/// 
		/// @param parent_size Size of parent element.
		/// @param element_size Size of the element.
		/// 
		/// @return Offset in pixels.
		int get(int parent_size, int element_size) const {
			return px + (int)(ps * parent_size + es * element_size);
		};

		/// Constructs a dimension from given values.
		/// 
		/// @param pixels Base value.
		/// @param parent Parent size scalar (0 by default).
		/// @param element Element size scalar (0 by default).
		Dim(int pixels = 0, float parent = 0.f, float self = 0.f) : px(pixels), ps(parent), es(self) {};

		/// Constructs a dimension with a base offset.
		static Dim from_px(int px) { return Dim(px, 0.f, 0.f); };
		/// Constructs a dimension with a parent size scalar.
		static Dim from_ps(float ps) { return Dim(0, ps, 0.f); };
		/// Constructs a dimension with an element size scalar.
		static Dim from_es(float es) { return Dim(0, 0.f, es); };

		/// Adds 2 dimensions.
		Dim operator+(const Dim& oth) const { return { px + oth.px, ps + oth.ps, es + oth.es }; };
		/// Subtracts 2 dimensions.
		Dim operator-(const Dim& oth) const { return { px - oth.px, ps - oth.ps, es - oth.es }; };
		/// Scale a dimension by a factor.
		Dim operator*(float scale) { return { (int)(px * scale), ps * scale, es * scale }; };
		/// Inverts a dimension.
		Dim operator-() const { return { -px, -ps, -es }; };

		/// Linearly interpolates between 2 dimensions.
		/// 
		/// @param a Starting dimension.
		/// @param b Ending dimension.
		/// @param t Interpolation progress.
		/// 
		/// @return Interpolated dimension.
		static Dim lerp(const Dim& a, const Dim& b, float t) { return a + (b - a) * t; };
	};
};

/// Converts a number into a dimension.
ui::Dim operator""px(unsigned long long i) { return ui::Dim::from_px((int)i); };
/// Converts a number into a dimension.
ui::Dim operator""ps(long double f) { return ui::Dim::from_ps(f); };
/// Converts a number into a dimension.
ui::Dim operator""es(long double f) { return ui::Dim::from_es(f); };
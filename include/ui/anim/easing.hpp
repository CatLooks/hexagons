#pragma once

namespace ui {
	/// Easing function type.
	///
	/// @param t Linear interpolation parameter.
	/// 
	/// @return Transformed interpolation parameter.
	typedef float (*Easing)(float t);

	/// Collection of easing functions.
	namespace Easings {
		/// Linear interpolation easing.
		float linear(float t);
		
		/// Quadratic in / linear out.
		float quadIn(float t);
		/// Linear in / quadratic out. 
		float quadOut(float t);
		/// Quadratic in & out.
		float quad(float t);

		/// Sine in / linear out.
		float sineIn(float t);
		/// Linear in / sine out.
		float sineOut(float t);
		/// Sine in & out.
		float sine(float t);
	};
};
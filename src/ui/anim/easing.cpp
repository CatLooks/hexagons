#include "ui/anim/easing.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

namespace ui {
	namespace Easings {
		// no easing
		float linear(float t) { return t; };

		// quad
		float quadIn(float t) { return t * t; };
		float quadOut(float t) { return t * (2.f - t); };
		float quad(float t) {
			if (t < 0.5f)
				return 2.f * t * t;
			float x = 2.f * (1.f - t);
			return 1.f - x * x * 0.5f;
		};

		// sine
		float sineIn(float t) { return 1.f - cosf(t * (float)M_PI_2); };
		float sineOut(float t) { return sinf(t * (float)M_PI_2); };
		float sine(float t) { return (1.f - cosf(t * (float)M_PI)) * 0.5f; };
	};
};
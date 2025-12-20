#pragma once

// include dependencies
#include "base.hpp"

namespace ui {
	/// Animation setter object.
	///
	/// Invokes a callback when the animator is ticked.
	class AnimSet : public Anim {
	public:
		using Setter = std::function<void(float t)>;

		/// Constructs a setter animation.
		///
		/// By default, passed interpolation parameter will be in range `[0, 1]`.
		///
		/// @param duration Animation duration.
		/// @param setter Setter function.
		AnimSet(const sf::Time& duration, Setter setter);

		/// Sets animation value range.
		///
		/// @param lower Lower range bound.
		/// @param upper Upper range bound.
		void setRange(float lower, float upper);

	protected:
		Setter _setter;     /// Setter callback.
		float _lower = 0.f; /// Lower interpolation bound.
		float _upper = 1.f; /// Upper interpolation bound.

		/// Invokes the setter.
		void onTick(float t) override;
	};
};
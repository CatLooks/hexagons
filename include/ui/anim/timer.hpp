#pragma once

// include dependencies
#include "base.hpp"

namespace ui {
	/// Animation timer.
	/// Uses animation framework to create timers.
	class AnimTimer : public Anim {
	protected:
		/// Does nothing.
		void onTick(float t) override;

	public:
		/// Constructs an animation timer.
		/// 
		/// @param duration Timer duration.
		/// @param call Timer callback.
		AnimTimer(const sf::Time& duration, std::function<void()> call);
	};
};
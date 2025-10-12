#include "ui/anim/timer.hpp"

namespace ui {
	// do nothing
	void AnimTimer::onTick(float t) {};

	/// Constructs an animation timer.
	AnimTimer::AnimTimer(const sf::Time& duration, std::function<void()> call) {
		_dur = duration.asSeconds();
		_end = call;
		_active = true;
	};
};
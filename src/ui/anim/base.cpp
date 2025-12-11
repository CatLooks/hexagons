#include "ui/anim/base.hpp"

namespace ui {
	/// Updates the animation target.
	void Anim::onTick(float t) {};

	/// Restarts the animation.
	void Anim::restart() {
		_time = 0.f;
		_active = true;
	};

	/// Ticks the animation.
	void Anim::update(const sf::Time& time) {
		// ignore if inactive
		if (!_active) return;

		// update timer
		_time += time.asSeconds();

		// handle animation end
		if (_time >= _dur) {
			_time = _dur;
			_active = false;
			onTick(1.f);
			if (_end) _end();
		}

		// tick animation
		else onTick(_ease(_time / _dur));
	};

	/// @return Whether the animation is active.
	bool Anim::active() const { return _active; };
	/// @return Time left to the end of animation.
	float Anim::left() const { return _active ? _dur - _time : 0.f; };

	/// Sets animation end callback.
	void Anim::setAfter(std::function<void()> call) {
		_end = call;
	};
	/// Adds animation end callback.
	void Anim::addAfter(std::function<void()> call) {
		// set callback if none set before
		if (!_end) {
			_end = call;
			return;
		};

		// create a callback chain
		std::function<void()> prev = _end;
		_end = [=]() { prev(); call(); };
	};

	/// Sets animation easing.
	Anim& Anim::setEasing(Easing easing) {
		_ease = easing;
		return *this;
	};
};
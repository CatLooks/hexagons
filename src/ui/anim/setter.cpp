#include "ui/anim/setter.hpp"
#include "ui/units.hpp"

namespace ui {
	/// Constructs a setter animation.
	AnimSet::AnimSet(const sf::Time& duration, Setter setter) {
		_dur = duration.asSeconds();
		_setter = setter;
		_active = true;
	};

	/// Sets animation value range.
	void AnimSet::setRange(float lower, float upper) {
		_lower = lower;
		_upper = upper;
	};

	/// Invokes the setter.
	void AnimSet::onTick(float t) {
		_setter(lerpf(_lower, _upper, t));
	};
};
#pragma once

// include dependencies
#include <SFML/System/Time.hpp>
#include <functional>
#include "easing.hpp"

namespace ui {
	/// Abstract animator object.
	class Anim {
	protected:
		float _time  = 0.f;             /// Time since animation start.
		float _dur   = 0.f;             /// Animation duration.
		Easing _ease = Easings::linear; /// Animation easing function.
		bool _active = false;           /// Whether the animation is active.
		std::function<void()> _end;     /// Animation end callback.

		/// Updates the animation target.
		/// @param t Interpolation progress.
		virtual void onTick(float t);

	public:
		bool looped = false; /// Whether to loop the animation.

	public:
		/// Restarts the animation.
		void restart();

		/// Ticks the animation.
		/// @param time Time elapsed since last frame.
		void update(const sf::Time& time);

		/// @return Whether the animation is active.
		bool active() const;
		/// @return Time left to the end of animation.
		float left() const;

		/// Sets animation end callback.
		void setAfter(std::function<void()> call);
		/// Adds animation end callback.
		void addAfter(std::function<void()> call);

		/// Sets animation easing.
		/// 
		/// @param easing Easing function.
		/// 
		/// @return Self-reference.
		Anim& setEasing(Easing easing);
	};
};
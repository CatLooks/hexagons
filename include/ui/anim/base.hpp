#pragma once

// include dependencies
#include <SFML/System/Time.hpp>
#include <functional>
#include "easing.hpp"

namespace ui {
	/// Abstract animator object.
	class Anim {
	public:
		/// Animation looping mode.
		enum Looping {
			None   = 0, /// Animation does not loop.
			Loop   = 1, /// Animation restarts after the loop.
			Bounce = 2, /// Animations loops and reverses.
		};

		Easing ease   = Easings::linear; /// Animation easing function.
		Looping mode  = None;            /// Animation looping mode.
		bool reversed = false;           /// Whether the animation runs in reverse.

	protected:
		bool _active  = false;           /// Whether the animation is active.
		float _time   = 0.f;             /// Time since animation start.
		float _dur    = 0.f;             /// Animation duration.
		std::function<void()> _end;      /// Animation end callback.

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

	protected:
		/// Returns interpolated animation progress.
		///
		/// @param t Raw animation progress.
		///
		/// @return Interpolated animation progress.
		float prog(float t);

		/// Updates the animation target.
		/// @param t Interpolation progress.
		virtual void onTick(float t);
	};
};
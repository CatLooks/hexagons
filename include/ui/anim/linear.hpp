#pragma once

// include dependencies
#include <SFML/Graphics/Color.hpp>
#include "base.hpp"
#include "ui/units.hpp"

namespace ui {
	/// Linear value animator object.
	/// 
	/// @tparam T Animated type, must be linear.
	/// @tparam lerp Linear interpolation function for the animated type.
	template <typename T, T lerp(T, T, float)>
	class AnimLinear : public Anim {
		static_assert(std::is_copy_constructible<T>());
		static_assert(std::is_copy_assignable<T>());

	protected:
		T* _target; /// Animation target.
		T _beg;     /// Target start value.
		T _end;     /// Target end value.

		/// Updates target's value.
		/// @param t Interpolation progress.
		void onTick(float t) override { *_target = lerp(_beg, _end, t); };

	public:
		/// Returns animation state at the start.
		const T& start_state() const {
			return _beg;
		};

		/// Returns animation state at the start.
		const T& end_state() const {
			return _end;
		};

		/// Constructs the linear animator object.
		/// 
		/// @param target Animation target.
		/// @param start Target start value.
		/// @param end Target end value.
		/// @param duration Animation duration.
		AnimLinear(T* target, const T& start, const T& end, sf::Time duration)
			: _target(target), _beg(start), _end(end)
		{
			_dur = duration.asSeconds();
			_active = true;
		};

		/// Constructs the linear animator object.
		/// 
		/// Animation starts from the current value of target.
		/// 
		/// @param target Animation target.
		/// @param end Target end value.
		/// @param duration Animation duration.
		static AnimLinear* to(T* target, const T& end, sf::Time duration) {
			return new AnimLinear(target, *target, end, duration);
		};

		/// Constructs the linear animator object.
		/// 
		/// Animation moves the value of the target from its starting value.
		/// 
		/// @param target Animation target.
		/// @param shift Target value shift.
		/// @param duration Animation duration.
		static AnimLinear* move(T* target, const T& shift, sf::Time duration) {
			return new AnimLinear(target, *target, *target + shift, duration);
		};
	};

	/// Integer animator class.
	using AnimInt = AnimLinear<int, lerpi>;
	/// Float animator class.
	using AnimFloat = AnimLinear<float, lerpf>;
	/// Color animator class.
	using AnimColor = AnimLinear<sf::Color, lerpc>;
	/// Dimension animator class.
	using AnimDim = AnimLinear<Dim, Dim::lerp>;
	/// Dimension vector animator class.
	using AnimVector = AnimLinear<DimVector, DimVector::lerp>;
};
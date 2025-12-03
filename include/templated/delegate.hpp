#pragma once

// include dependencies
#include <functional>
#include <vector>
#include <type_traits>

/// Generic delegate template.
/// 
/// @tparam F Action function signature.
/// @tparam A Accumulator object type.
template <typename F, typename A = void> class Delegate;

/// Delegate of procedural actions.
/// 
/// @tparam Args Action parameter type list.
template <typename... Args> class Delegate<void(Args...)> {
public:
	/// Delegate function signature.
	using Action = std::function<void(Args...)>;

private:
	std::vector<Action> _list; /// Action list.

public:
	/// Removes all delegate actions.
	void clear() {
		_list.clear();
	};

	/// Adds a delegate action to callback chain.
	/// 
	/// @param action Action function.
	void add(const Action& action) {
		_list.push_back(action);
	};

	/// Invokes the delegate.
	/// 
	/// Actions are invoked in their order of addition.
	/// 
	/// @param args Action argument list.
	void invoke(Args... args) const {
		for (const auto& f : _list)
			f(args...);
	};
};

/// Invalid delegate.
///
/// Reduction is only allowed for non-void action return types.
template <typename A, typename... Args> class Delegate<void(Args...), A>;

/// Delegate that reduces results of actions.
/// 
/// @tparam A Accumulator object type.
/// @tparam R Action return type.
/// @tparam Args Action parameter type list.
template <typename A, typename R, typename... Args> class Delegate<R(Args...), A> {
public:
	/// Delegate function signature.
	using Action = std::function<R(Args...)>;
	/// Delegate function result reducer function.
	/// 
	/// @param acc Accumulator reference.
	/// @param value Reduced value from action invocation.
	/// 
	/// @return `true` to stop invocation chain, `false` to continue.
	using Reduce = std::function<bool(A& acc, const R& value)>;

private:
	std::vector<Action> _list; /// Action list.
	Reduce _reduce;            /// Reducer function.

public:
	/// Reducing delegate constructor.
	/// 
	/// Reducer function should return `true` to stop invocation chain, `false` to continue.
	/// 
	/// @param reduce Reducer function.
	Delegate(Reduce reduce) : _reduce(reduce) {};

	/// Removes all delegate actions.
	void clear() {
		_list.clear();
	};

	/// Adds a delegate action to callback chain.
	/// 
	/// @param action Action function.
	void add(const Action& action) {
		_list.push_back(action);
	};

	/// Invokes the delegate.
	/// 
	/// Actions are invoked in their order of addition.
	/// 
	/// @param acc Initial accumulator value.
	/// @param args Action argument list.
	/// 
	/// @return Value reduced from invoked actions.
	A reduce(A acc, Args... args) const {
		for (const auto& f : _list)
			if (_reduce(acc, f(args...)))
				break;
		return acc;
	};

	/// Invokes the delegate.
	/// 
	/// Actions are invoked in their order of addition.
	/// 
	/// Default value of `R` will be used as initial accumulator value.
	/// 
	/// @param args Action argument list.
	/// 
	/// @return Value reduced from invoked actions.
	A reduce(Args... args) const {
		static_assert(std::is_default_constructible_v<A>, "accumulator must have a default constructor");
		return reduce(A(), args...);
	};
};
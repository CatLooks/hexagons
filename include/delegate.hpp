#pragma once

// include dependencies
#include <functional>
#include <vector>
#include <type_traits>

/// General delegate template.
/// 
/// @tparam F Action function signature.
template <typename F> class Delegate;

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
	void add(Action&& action) {
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

/// Delegate that reduces results of actions.
/// 
/// @tparam R Action return type.
/// @tparam Args Action parameter type list.
template <typename R, typename... Args> class Delegate<R(Args...)> {
public:
	/// Delegate function signature.
	using Action = std::function<R(Args...)>;
	/// Delegate function result reducer function.
	/// 
	/// @param acc Accumulator reference.
	/// @param value Reduced value from action invocation.
	/// 
	/// @return `true` to stop invocation chain, `false` to continue.
	using Reduce = std::function<bool(R& acc, const R& value)>;

private:
	std::vector<Action> _list; /// Action list.
	Reduce _reduce;            /// Reducer function.

public:
	/// Reducing delegate constructor.
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
	void add(Action&& action) {
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
	R reduce(R acc, Args... args) const {
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
	R reduce(Args... args) const {
		static_assert(std::is_default_constructible_v<R>, "return type must have a default constructor");
		return reduce(R(), args...);
	};
};
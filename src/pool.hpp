#pragma once

// include dependencies
#include <vector>
#include <list>

/// Object pool.
/// Objects stay at the same index all the time.
template <typename T> class Pool {
protected:
	std::vector<T> _storage;  /// Object storage vector.
	std::list<size_t> _empty; /// List of empty indices.

public:
	/// Pushes an item on the pool.
	/// @return Item index.
	size_t push(T item) {
		if (_empty.empty()) {
			_storage.push_back(item);
			return _storage.size() - 1;
		};
		size_t idx = _empty.front();
		_empty.pop_front();
		_storage[idx] = item;
		return idx;
	};

	/// Removes an item from the pool.
	void pop(size_t idx) {
		if (idx + 1 == _storage.size()) {
			_storage.pop_back();
			return;
		};
		_empty.push_back(idx);
	};

	/// Returns a reference to an item in the pool.
	T& operator[](size_t idx) {
		return _storage.at(idx);
	};
	const T& operator[](size_t idx) const {
		return _storage.at(idx);
	};

	/// Returns pool item count.
	size_t count() const {
		return _storage.size() - _empty.size();
	};
};
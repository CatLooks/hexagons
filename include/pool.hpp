#pragma once

// include dependencies
#include <vector>
#include <set>
#include <exception>
#include <format>

/// Templated pool object.
/// Continuous array of elements that don't change their position during their lifetime.
template <typename T, class Allocator = std::allocator<T>> class Pool {
private:
	std::vector<T>   _storage; /// Item storage vector.
	std::vector<bool> _status; /// Item status vector.
	std::set<size_t> _deleted; /// Deleted indices list.

public:
	/// Exception raised by `at()`.
	class access_exc : std::exception {
	private:
		mutable std::string _msg; /// Message string.

	public:
		const Pool* pool; /// Pool pointer.
		size_t       idx; /// Accessed index.
		bool     deleted; /// Reason for exception.

		/// Default constructor.
		access_exc(const Pool* pool, size_t idx, bool deleted)
			: pool(pool), idx(idx), deleted(deleted) {};

		/// Returns exception string.
		const char* what() const noexcept override {
			_msg = std::format(
				"attempted accessing {} index {} from pool at {}",
				(deleted ? "deleted" : "out-of-range"), idx, (void*)pool
			);
			return _msg.c_str();
		};
	};
	/// Exception raised by `pop()`.
	class pop_exc : std::exception {
	private:
		mutable std::string _msg; /// Message string.

	public:
		const Pool* pool; /// Pool pointer.
		size_t       idx; /// Popped index.
		bool     deleted; /// Reason for exception.

		/// Default constructor.
		pop_exc(const Pool* pool, size_t idx, bool deleted)
			: pool(pool), idx(idx), deleted(deleted) {};

		/// Returns exception string.
		const char* what() const noexcept override {
			_msg = std::format(
				"attempted popping {} index {} from pool at {}",
				(deleted ? "deleted" : "out-of-range"), idx, (void*)pool
			);
			return _msg.c_str();
		};
	};

	/// Constructs an empty pool.
	Pool() {};
	/// Constructs a pool from an initializer list.
	Pool(std::initializer_list<T> list): _storage(list), _status(list.size(), true) {};

	/// Returns the amount of items in pool.
	size_t count() const { return _storage.size() - _deleted.size(); };
	/// Checks whether the pool is empty.
	bool empty() const { return count() == 0; };

	/// Returns a reference to an item in the pool.
	///
	/// Exceptions: `access_exc`.
	///
	/// @param idx Item index in pool.
	T& at(size_t idx) {
		if (idx >= _storage.size())
			throw access_exc(this, idx, false);
		if (!_status[idx])
			throw access_exc(this, idx, true);
		return _storage[idx];
	};
	const T& at(size_t idx) const {
		if (idx >= _storage.size())
			throw access_exc(this, idx, false);
		if (!_status[idx])
			throw access_exc(this, idx, true);
		return _storage[idx];
	};

	/// Pushes an item to the pool.
	///
	/// @param item Added item.
	///
	/// @return Item index in pool.
	size_t push(T&& item) {
		// check if no deleted items
		if (_deleted.empty()) {
			// append element to storage end
			size_t idx = _storage.size();
			_storage.push_back(item);
			_status.push_back(true);
			return idx;
		};

		// pull out smallest deleted index
		size_t idx = *_deleted.begin();
		_deleted.erase(_deleted.begin());

		// overwrite position with item
		_storage[idx] = item;
		_status[idx] = true;
		return idx;
	};
	size_t push(const T& item) {
		T temp = item;
		return push(std::move(temp));
	};

	/// Pops out an item from the pool.
	///
	/// Exceptions: `pop_exc`.
	///
	/// @param idx Item index in pool.
	///
	/// @return Popped-out item.
	T pop(size_t idx) {
		// check if item under index exists
		if (idx >= _storage.size())
			throw pop_exc(this, idx, false);
		if (!_status[idx])
			throw pop_exc(this, idx, true);

		// move out item
		T item = std::move(_storage[idx]);
		if (idx + 1 == _storage.size()) {
			_storage.pop_back();
			_status.pop_back();

			// pop deleted indices
			while (!_status.back()) {
				_storage.pop_back();
				_status.pop_back();
			};
		} else {
			// mark as deleted
			_deleted.insert(idx);
			_status[idx] = false;
		};
		return item;
	};

	// friend iterators
	class iter;
	friend iter;
	class const_iter;
	friend const_iter;

	/// Pool iterator class.
	/// Allows for item modification.
	class iter {
		friend Pool;

	private:
		Pool* _pool; /// Iterated pool.
		size_t _idx; /// Current item index.

		/// Hidden constructor.
		///
		/// @param pool Iterated pool.
		/// @param idx Starting index.
		iter(Pool* pool, size_t idx)
			: _pool(pool), _idx(idx) { _seek(); };

		/// Increments index until an occupied item is found.
		void _seek() {
			while (valid()) {
				if (_pool->_status[_idx])
					break;
				_idx++;
			};
		};

	public:
		/// Returns current iterated item.
		T& operator*() const { return _pool->_storage[_idx]; };
		/// Returns pointer to current iterated item.
		T* operator->() const { return &_pool->_storage[_idx]; };

		/// Checks if iterator points inside of a pool.
		bool valid() const { return _idx < _pool->_storage.size(); };
		/// Checks if iterators point at the same index.
		bool operator==(const iter& oth) const { return valid() ? (_idx == oth._idx) : !oth.valid(); };
		/// Checks if iterators point at different indices.
		bool operator!=(const iter& oth) const { return !(*this == oth); };

		/// Advances iterator to next item.
		/// Iterator state is returned after advancing.
		iter operator++() { _idx++; _seek(); return *this; };
		/// Advances iterator to next item.
		/// Iterator state is returned before advancing.
		iter operator++(int) { iter state = *this; _idx++; _seek(); return state; };

		/// Returns current item index.
		size_t index() const { return _idx; };

		/// Pops out current iterated item.
		/// After this, iterator will point to a deleted item, advance to next item before accessing.
		T pop() const { return _pool->pop(_idx); };
	};
	/// Pool constant iterator class.
	/// Forbids item modification.
	class const_iter {
		friend Pool;

	private:
		const Pool* _pool; /// Iterated pool.
		size_t       _idx; /// Current item index.

		/// Hidden constructor.
		///
		/// @param pool Iterated pool.
		/// @param idx Starting index.
		const_iter(const Pool* pool, size_t idx)
			: _pool(pool), _idx(idx) { _seek(); };

		/// Increments index until an occupied item is found.
		void _seek() {
			while (_idx < _pool->_storage.size()) {
				if (_pool->_status[_idx])
					break;
				_idx++;
			};
		};

	public:
		/// Returns current iterated item.
		const T& operator*() const { return _pool->_storage[_idx]; };
		/// Returns pointer to current iterated item.
		const T* operator->() const { return &_pool->_storage[_idx]; };

		/// Checks if iterators point at the same index.
		bool operator==(const const_iter& oth) const { return _idx == oth._idx; };
		/// Checks if iterators point at different indices.
		bool operator!=(const const_iter& oth) const { return _idx != oth._idx; };

		/// Advances iterator to next item.
		/// Iterator state is returned after advancing.
		const_iter operator++() { _idx++; _seek(); return *this; };
		/// Advances iterator to next item.
		/// Iterator state is returned before advancing.
		const_iter operator++(int) { const_iter state = *this; _idx++; _seek(); return state; };

		/// Returns current item index.
		size_t index() const { return _idx; };
	};

	/// Returns iterator to start of the pool.
	iter begin() { return iter(this, 0); };
	/// Returns iterator to end of the pool.
	iter end() { return iter(this, _storage.size()); };

	/// Returns iterator to start of the pool.
	const_iter begin() const { return const_iter(this, 0); };
	/// Returns iterator to end of the pool.
	const_iter end() const { return const_iter(this, _storage.size()); };

	/// Returns constant iterator to start of the pool.
	const_iter cbegin() const { return begin(); };
	/// Returns constant iterator to end of the pool.
	const_iter cend() const { return end(); };
};
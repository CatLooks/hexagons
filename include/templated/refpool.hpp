#pragma once

// include dependencies
#include <vector>
#include <set>

/// Reference pool container object.
/// 
/// Stores items in a continuous memory block.
/// Items are deleted once there are no references to it.
/// 
/// @tparam T Stored item type.
template <typename T> class RefPool {
public:
	// forward declare item
	class Item; friend Item;

private:
	/// Item storage info.
	struct V {
		T      item; /// Item data.
		size_t refs; /// Reference count.
	};

	std::vector<V>   _storage; /// Pool storage.
	std::set<size_t> _deleted; /// Deleted indices.
	bool _void = false;        /// Whether to void deletion calls.

public:
	/// Shared pool item reference.
	///
	/// Provides access to item inside the pool.
	/// If all shared references are destroyed, the item will be deleted.
	class Share {
		friend RefPool;

	private:
		RefPool* _pool; /// Pool reference.
		size_t  _index; /// Item index.

	public:
		/// Move constructor.
		Share(Share&& item) noexcept : _pool(item._pool), _index(item._index) {
			item._pool = nullptr;
		};
		/// Move assignment.
		Share& operator=(Share&& item) noexcept {
			_pool = item._pool;
			_index = item._index;
			item._pool = nullptr;
			return *this;
		};

		/// Copy constructor.
		Share(const Share& item) noexcept : _pool(item._pool), _index(item._index) {
			if (_pool) _pool->push(_index);
		};
		/// Copy assignment.
		Share& operator=(const Share& item) noexcept {
			if (this != &item) {
				if (_pool) _pool->pop(_index);
				{
					_pool = item._pool;
					_index = item._index;
				};
				if (_pool) _pool->push(_index);
			};
			return *this;
		};

		/// Destroys an item reference.
		///
		/// If no references are left, the item will be deleted.
		~Share() {
			if (_pool) _pool->pop(_index);
		};

		/// Constructs an empty reference.
		Share() : _pool(nullptr), _index(0) {};

	private:
		/// Constructs an item reference.
		///
		/// Only accessible from a pool object.
		///
		/// @param pool Pool reference.
		/// @param index Item index.
		Share(RefPool* pool, size_t index) : _pool(pool), _index(index) {};

	public:
		/// Checks if 2 references are equal.
		bool operator==(const Share& oth) const {
			return _pool == oth._pool && _index == oth._index;
		};
		/// Checks if 2 references are not equal.
		bool operator!=(const Share& oth) const {
			return !(*this == oth);
		};

	public:
		/// Checks whether the object does not hold a reference.
		bool null() const { return _pool == nullptr; };
		/// Checks whether the object does not hold a reference.
		bool operator!() const { return null(); };
		/// Checks whether the object holds a reference.
		operator bool() const { return !null(); };

		/// Returns item index.
		size_t index() const { return _index; };

		/// Returns the referenced item.
		T& operator*() const { return _pool->_storage[_index].item; };
		/// Returns a pointer to the referenced item.
		T* operator->() const { return &_pool->_storage[_index].item; };
	};

	/// Prevents any further deletion calls.
	~RefPool() { _void = true; };

	/// Adds an item to the pool.
	///
	/// @param item Moved item.
	///
	/// @return Item reference object.
	[[nodiscard]] Share add(T&& item) {
		if (_deleted.empty()) {
			// push item to storage end
			size_t idx = _storage.size();
			_storage.push_back({ item, 1 });
			return { this, idx };
		};

		// pull a deleted index
		size_t idx = *_deleted.begin();
		_deleted.erase(_deleted.begin());

		// overwrite the deleted space
		_storage[idx] = { item, 1 };
		return { this, idx };
	};

	/// Adds an item to the pool.
	///
	/// @param item Copied item.
	///
	/// @return Item reference object.
	[[nodiscard]] Share add(const T& item) {
		T copy = item;
		return add(std::move(copy));
	};

	/// Rebases the reference to the pool.
	/// 
	/// This method should only be used after copying or moving the original pool.
	/// 
	/// @param item Item reference object.
	void rebase(Share& item) {
		if (item) item._pool = this;
	};

	/// Returns current pool capacity.
	size_t capacity() const { return _storage.size(); };
	/// Returns amount of items in the pool.
	size_t count() const { return _storage.size() - _deleted.size(); };

protected:
	/// Increments the amount of references to an item.
	/// 
	/// @param idx Item index.
	void push(size_t idx) {
		_storage[idx].refs++;
	};

	/// Decrements the amount of references to an item.
	///
	/// If the amount of references goes to 0, the item will be deleted.
	/// 
	/// @param idx Item index.
	void pop(size_t idx) {
		// ignore if pool has been destroyed already
		if (_void) return;

		// decrement reference count
		// return if references left
		if (--_storage[idx].refs > 0) return;
		
		// delete item
		if (idx + 1 == _storage.size()) {
			// delete item at storage end
			_storage.pop_back();

			// check for trailing deleted indices
			while (!_deleted.empty()) {
				// get farthest deleted index
				size_t idx = *_deleted.rbegin();
				if (idx + 1 != _storage.size())
					[[unlikely]] break;

				// delete last item in storage
				_deleted.erase(std::next(_deleted.rbegin()).base());
				_storage.pop_back();
			};
			return;
		};

		// mark index as deleted
		_deleted.insert(idx);
	};
};
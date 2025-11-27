#pragma once

// include dependencies
#include <vector>
#include <set>

/// Pool container object.
///
/// Stores objects in a continuous memory block which guarantees O(1) access time.
/// Stored items have an access index which does *not* change.
/// Items are stored in arbitrary order.
template <typename T> class Pool {
public:
	// forward declarations
	class Ref;  friend Ref;
	class Item; friend Item;

private:
	std::vector<T>   _storage; /// Pool storage.
	std::set<size_t> _deleted; /// Deleted pool indices.

public:
	/// Pool item reference object.
	/// Provides access to item inside a pool.
	/// Does not own the referenced item.
	class Ref {
		friend Pool;
		friend Item;

	private:
		T*     _ref; /// Item reference.
		Pool* _pool; /// Pool reference.

		/// Constructs an empty reference.
		Ref(): _ref(nullptr), _pool(nullptr) {};

		/// Constructs a pool item reference.
		///
		/// Only accessible from a pool object.
		///
		/// @param pool Pool reference.
		/// @param index Item index.
		Ref(Pool* pool, size_t index): _ref(&pool->_storage[index]), _pool(pool) {};

	public:
		/// Checks whether the object does not hold a reference.
		bool null() const { return _pool == nullptr; };
		/// Checks whether the object does not hold a reference.
		bool operator!() const { return null(); };
		/// Checks whether the object holds a reference.
		operator bool() const { return !null(); };

		/// Returns item index.
		size_t index() const { return _ref - _pool->_storage.data(); };

		/// Returns the referenced item.
		T& operator*() const { return *_ref; };
		/// Returns a pointer to the referenced item.
		T* operator->() const { return _ref; };
	};

	/// Owning pool item reference object.
	/// Takes ownership of an item inside a pool.
	/// Upon destruction deletes the item from the pool.
	class Item : public Ref {
		friend Pool;

	private:
		/// Constructs an empty item.
		Item(): Ref() {};

		/// Constructs a pool item reference.
		///
		/// Only accessible from a pool object.
		///
		/// @param pool Pool reference.
		/// @param index Item index.
		Item(Pool* pool, size_t index): Ref(pool, index) {};

	public:
		/// Disables copying.
		Item (const Item&) = delete;
		/// Disables copying.
		void operator=(const Item&) = delete;

		/// Deletes referenced item from the pool.
		~Item() { this->_pool->pop(this->index()); };

		/// Constructs a reference to the item.
		Ref ref() const { return (Ref)(*this); };
	};

	/// Adds an item to the pool.
	///
	/// @param item Moved item.
	///
	/// @return Item reference object.
	Item add(T&& item) {
		if (_deleted.empty()) {
			// push item to storage end
			size_t idx = _storage.size();
			_storage.push_back(item);
			return { this, idx };
		};

		// pull a deleted index
		size_t idx = *_deleted.begin();
		_deleted.erase(_deleted.begin());

		// overwrite the deleted space
		_storage[idx] = item;
		return { this, idx };
	};

	/// Adds an item to the pool.
	///
	/// @param item Copied item.
	///
	/// @return Item reference object.
	Item add(const T& item) {
		T copy = item;
		return push(std::move(copy));
	};

	/// Returns current pool capacity.
	size_t capacity() const { return _storage.size(); };
	/// Returns amount of items in the pool.
	size_t count() const { return _storage.size() - _deleted.size(); };

protected:
	/// Deletes a referenced item from the pool.
	///
	/// @param idx Deleted index.
	void pop(size_t idx) {
		if (idx + 1 == _storage.size()) {
			// delete item at storage end
			_storage.pop_back();
			return;
		};

		// mark index as deleted
		_deleted.insert(idx);
	};
};
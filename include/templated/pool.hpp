#pragma once

// include dependencies
#include <vector>
#include <set>

/// Pool container object.
///
/// Stores items in a continuous memory block.
/// Items can be accessed by static references.
/// Items are stored in arbitrary order.
/// 
/// @tparam T Stored item type.
template <typename T> class Pool {
public:
	// forward declarations
	class Ref;  friend Ref;
	class Item; friend Item;

private:
	std::vector<T>   _storage; /// Pool storage.
	std::set<size_t> _deleted; /// Deleted pool indices.
	bool _void = false;        /// Whether to void deletion calls.

public:
	/// Pool item reference object.
	/// 
	/// Provides access to item inside a pool.
	/// Does not own the referenced item.
	class Ref {
		friend Pool;
		friend Item;

	private:
		Pool* _pool; /// Pool reference.
		size_t _idx; /// Item index.

	public:
		/// Move constructor.
		Ref(Ref&& ref) noexcept : _pool(ref._pool), _idx(ref._idx) {
			ref._pool = nullptr;
		};
		/// Move assignment.
		Ref& operator=(Ref&& ref) noexcept {
			_pool = ref._pool;
			_idx = ref._idx;
			ref._pool = nullptr;
			return *this;
		};

		/// Constructs an empty reference.
		Ref(): _pool(nullptr), _idx(0) {};

	private:
		/// Constructs a pool item reference.
		///
		/// Only accessible from a pool object.
		///
		/// @param pool Pool reference.
		/// @param index Item index.
		Ref(Pool* pool, size_t index): _pool(pool), _idx(index) {};

	public:
		/// Checks if 2 references are equal.
		bool operator==(const Ref& oth) const {
			return _pool == oth._pool && _idx == oth._idx;
		};
		/// Checks if 2 references are not equal.
		bool operator!=(const Ref& oth) const {
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
		size_t index() const { return _idx; };

		/// Returns the referenced item.
		T& operator*() const { return _pool->_storage[_idx]; };
		/// Returns a pointer to the referenced item.
		T* operator->() const { return &_pool->_storage[_idx]; };
	};

	/// Owning pool item reference object.
	/// 
	/// Takes ownership of an item inside a pool.
	/// Upon destruction deletes the item from the pool.
	class Item : public Ref {
		friend Pool;

	public:
		/// Constructs an empty item.
		Item() : Ref() {};

	private:
		/// Constructs a pool item reference.
		///
		/// Only accessible from a pool object.
		///
		/// @param pool Pool reference.
		/// @param index Item index.
		Item(Pool* pool, size_t index): Ref(pool, index) {};

	public:
		/// Disables copying.
		Item(const Item&) = delete;
		/// Disables copying.
		Item& operator=(const Item&) = delete;
		/// Default move constructor.
		Item(Item&&) noexcept = default;
		/// Default move assignment.
		Item& operator=(Item&&) noexcept = default;

		/// Deletes referenced item from the pool.
		~Item() { if (this->_pool) this->_pool->pop(this->_idx); };

		/// Constructs a reference to the item.
		[[nodiscard]] Ref ref() const { return (Ref)(*this); };
	};

	/// Prevents any further deletion calls.
	~Pool() { _void = true; };

	/// Adds an item to the pool.
	///
	/// @param item Moved item.
	///
	/// @return Item reference object.
	[[nodiscard]] Item add(T&& item) {
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
	[[nodiscard]] Item add(const T& item) {
		T copy = item;
		return add(std::move(copy));
	};

	/// Rebases the reference to the pool.
	/// 
	/// This method should only be used after copying or moving the original pool.
	/// 
	/// @param ref Item reference object.
	void rebase(Ref& ref) {
		if (ref) ref._pool = this;
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
		// ignore if pool has been destroyed already
		if (_void) return;
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
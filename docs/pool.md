# Pool

Pool is a generic collection that stores items in a continuous chunk of memory, being able to be indexed and accessed in constant time.

Additionally, pool allows for items to be deleted from any index in constant amortized time.

## Constructors

| Parameters | Description |
|-|-|
| `() -> Pool<T>` | Constructs an empty pool. |
| `(list: std::initializer_list<T>) -> Pool<T>` | Constructs a pool from an initializer list. |

## Access

Access methods provide property-like methods to access pool items and pool properties.

| Method | Parameters | Description |
|-|-|-|
| `empty` | `() -> bool` | Checks whether the pool is empty. |
| `count` | `() -> size_t` | Returns the amount of items currently in pool. |
| `at` | `(idx: size_t) -> T&` | Returns a reference to an item. <br> May throw [`Pool::access_exc`](##exceptions). |

## Modification

Modification methods provide a way to add or remove items from the pool.

| Method | Parameters | Description |
|-|-|-|
| `push` | `(item: T) -> size_t` | Inserts an item to the pool and returns its index. <br> *The item in pool will always be available under this index.* |
| `pop` | `(idx: size_t) -> T` | Pops out an item from the pool using its index. <br> May throw [`Pool::pop_exc`](##exceptions). |

## Exceptions

Exceptions can be thrown when trying to access a *deleted* or *out-of-range* index.

These exceptions can be thrown by `Pool::at` (`Pool::access_exc`) and `Pool::pop` (`Pool::pop_exc`).

Both exceptions contain:
| Field | Description |
|-|-|
| `pool: const Pool*` | Accessed pool pointer. |
| `idx: size_t` | Accessed index. |
| `deleted: bool` | Reason for exception. <br> `true` - Accessing a deleted index. <br> `false` - Accessing index out-of-range. |

## Iterators

Iterators are objects that allow iterating through a pool. Pool implements:
* `Pool::const_iter` - constant iterator, iterated items are read-only.
* `Pool::iter` - mutable iterator, iterated items can be modified.

### Extension methods

| Method | Parameters | Description |
|-|-|-|
| `index` | `() -> size_t` | Returns the index of currently iterated item. |
| `pop` | `() -> T` | Pops out currently iterated item. <br> This will invalidate currently pointed item. <br> Advance to next item before performing any other reads. <br> *Available only for `Pool::iter`. |

### Iterator generators

| Method | Parameters | Description |
|-|-|-|
| `begin` | `() -> iter` | Returns iterator *(mutable and constant)* to first item in pool. |
| `end` | `() -> iter` | Returns iterator *(mutable and constant)* to first position outside of pool. |
| `cbegin` | `() -> const_iter` | Returns a constant iterator to first item in pool. |
| `cend` | `() -> const_iter` | Returns a constant iterator to first position outside of pool. |

<hr>

## Usage examples

### Adding & removing items
```cpp
Pool<int> pool;        // empty

// pushing new items
auto i = pool.push(1); // (i = 0) | 1
auto j = pool.push(3); // (j = 1) | 1 3
auto k = pool.push(7); // (k = 2) | 1 3 7

// popping existing items
auto a = pool.pop(j);  // (a = 3) | 1 - 7
auto b = pool.pop(i);  // (b = 1) | - - 7

// auto-shrinking
auto l = pool.push(6); // (l = 0) | 6 - 7
auto c = pool.pop(k);  // (c = 7) | 6
```

### Iterating through a pool
```cpp
// example pool
Pool<char> pool = {'T', 'e', 'x', 's', 't'};

pool.pop(2); // removed 'x'
             // iteration still works normally

// implicit iteration
for (auto value : pool)
	printf("%c", value);
printf("\n");

// explicit iteration
for (auto it = pool.begin(); it.valid(); it++)
	printf("[%d] = %d\n", it.index(), *it);
```
Output:
```c
Test
[0] = T
[1] = e
[3] = s // (!) indices in pool
[4] = t
```

### Removing elements during iteration
```cpp
// example pool
Pool<int> pool = {1, 2, 3, 5, 8, 13, 21, 34};
for (auto it = pool.begin(); it != pool.end(); it++) {
	// remove all even numbers
	if (*it % 2 == 0)
		it.pop();
};

// print leftover numbers
for (int i : pool)
	printf("%d ", i);
```
Output:
```c
1 3 5 13 21
```

### Copying items from pool
```cpp
// example pool
Pool<char> pool = {'A', '.', 'B', '.', 'C'};
for (auto it = pool.begin(); it != pool.end(); it++) {
	// remove all '.'
	if (*it == '.') it.pop();
};

// create resulting character array
char* arr = new char[pool.count() + 1];
{
	size_t idx = 0;
	for (char c : pool)
		arr[idx++] = c;
	arr[idx] = '\0';
};
printf("%s\n", arr);
delete[] arr;
```
Output:
```
ABC
```
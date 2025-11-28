#pragma once

// include dependencies
#include <SFML/Graphics/Rect.hpp>
#include <memory>
#include "hex.hpp"

/// 2D hex array container.
///
/// Every even row does not have the last hex.
class HexArray {
public:
	/// Neighbor tile index.
	///   5   0
	///    \ /
	/// 4 - # - 1
	///    / \
	///   3   2
	enum nbi_t {
		UpperRight = 0, /// Neighbor on upper right.
		Right      = 1, /// Neighbor on right.
		LowerRight = 2, /// Neighbor on lower right.
		LowerLeft  = 3, /// Neighbor on lower left.
		Left       = 4, /// Neighbor on left.
		UpperLeft  = 5, /// Neighbor on upper left.
	};

	/// Returns neighbor position of a tile.
	///
	/// @param pos Tile position.
	/// @param nbi Neighbor tile index.
	///
	/// @return Neighbor tile position.
	static sf::Vector2i neighbor(sf::Vector2i pos, nbi_t nbi);

private:
	Hex*        _tiles {}; /// Tile container.
	sf::Vector2i _size {}; /// Array size.

public:
	/// Constructs an empty array.
	HexArray();
	/// Destroys the array.
	~HexArray();

	/// Move constructor.
	HexArray(HexArray&& array) noexcept;
	/// Move assignment.
	HexArray& operator=(HexArray&& array) noexcept;

	/// Disabled copying.
	HexArray(const HexArray&) = delete;
	/// Disabled copying.
	HexArray& operator=(const HexArray&) = delete;

public:
	/// Clears the array.
	///
	/// The size is set to zero.
	void clear();

	/// Empties the array and sets the new size.
	/// 
	/// @param size Array size.
	void empty(sf::Vector2i size);

	/// Resizes the array.
	/// 
	/// Old tiles are preserved.
	/// New tiles are empty.
	/// 
	/// @param rect New map size relative to current rectangle.
	void resize(sf::IntRect rect);

	/// Returns array size.
	sf::Vector2i size() const;
	/// Returns hex count.
	size_t count() const;

	/// Checks if a position is within the map.
	///
	/// @param pos Tile position.
	bool contains(sf::Vector2i pos) const;

protected:
	/// Unsafe tile look-up.
	/// 
	/// @param pos Tile position.
	/// 
	/// @return Tile reference.
	Hex& ats(sf::Vector2i pos) const;

public:
	/// Returns a reference to a tile at position.
	///
	/// If the position is out-of-bounds, a `nullptr` will be returned.
	///
	/// @param pos Tile position.
	///
	/// @return Tile reference.
	Hex* at(sf::Vector2i pos) const;
	/// Returns a reference to a tile at position.
	///
	/// If the position is out-of-bounds, a `nullptr` will be returned.
	///
	/// @param pos Tile position.
	///
	/// @return Tile reference.
	Hex* operator[](sf::Vector2i pos) const;
};
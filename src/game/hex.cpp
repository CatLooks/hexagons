#include "game/hex.hpp"

/// Checks if a tile is ground-like.
bool Hex::isSolid() const {
	return type == Ground || type == Bridge;
};
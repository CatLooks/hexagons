#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <deque>
#include <memory>
#include <optional>
#include "game/logic/skill_types.hpp"

// map forward declaration
class Map;

/// Abstract reversible game move.
struct Move {
	sf::Vector2i  skill_pos {}; /// Skill origin position.
	Skills::Type skill_type {}; /// Skill type.
	uint8_t  skill_cooldown {}; /// Skill cooldown.

	/// Applies the move.
	///
	/// @param map Game map reference.
	virtual void onApply(Map* map) = 0;

	/// Reverts the move.
	///
	/// @param map Game map reference.
	virtual void onRevert(Map* map) = 0;

	/// Returns tile to select after applying the move.
	virtual std::optional<sf::Vector2i> applyCursor();

	/// Returns tile to select after reverting the move.
	virtual std::optional<sf::Vector2i> revertCursor();

	/// Applies the move.
	///
	/// @param map Game map reference.
	void apply(Map* map);

	/// Reverts the move.
	///
	/// @param map Game map reference.
	void revert(Map* map);
};

/// Specific moves namespace.
namespace Moves {};
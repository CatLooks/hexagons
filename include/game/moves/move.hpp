#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <deque>
#include <memory>
#include <optional>
#include <variant>

#include "game/logic/skill_types.hpp"
#include "game/troop.hpp"
#include "game/build.hpp"
#include "game/plant.hpp"

#include "dev/dev_move.hpp"
#include <format>

// map forward declaration
class Map;

/// Abstract reversible game move.
///
/// Fields prefixed with `a_` are set with `apply()` method.
struct Move : dev::Move {
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
namespace Moves {
	/// Entity state description type.
	using EntState = std::variant<std::monostate, Troop, Build, Plant>;

	/// Returns string representation of an entity.
	/// 
	/// @param entity Entity variant pointer.
	/// 
	/// @return String representation.
	std::string str_ent(const EntState* entity);
};
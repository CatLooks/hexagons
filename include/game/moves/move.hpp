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
#include "game/hex.hpp"

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
	/// Empty entity state.
	struct Empty {
		sf::Vector2i pos; /// "Entity" position.
	};

	/// Entity state description type.
	using EntState = std::variant<Empty, Troop, Build, Plant>;

	/// Returns entity state of a hex.
	///
	/// @param hex Hex reference.
	/// @param pos Hex position.
	///
	/// @return Hex entity state.
	EntState store_entity(const Hex* hex, sf::Vector2i pos);

	/// Places an entity on a map.
	///
	/// @param entity Entity state.
	/// @param map Map reference.
	void place_entity(const EntState* entity, Map* map);

	/// Returns entity state's position.
	/// 
	/// @param entity Entity state.
	sf::Vector2i entity_pos(const EntState* entity);

	/// Returns string representation of an entity.
	/// 
	/// @param entity Entity variant pointer.
	std::string str_ent(const EntState* entity);

	/// Returns string representation of an effect list.
	/// 
	/// @param list Entity effect list.
	std::string str_eff(const Entity::Effects& list);
};
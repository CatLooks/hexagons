#pragma once

// include dependencies
#include "template.hpp"
#include "game/logic/skill_list.hpp"

/// Bot AI related stuff.
namespace ai {
	/// Returns available moves for a skill.
	///
	/// @param map Map reference.
	/// @param skill Skill description.
	/// @param state Skill state.
	/// @param hex Origin hex.
	/// @param pos Origin position.
	/// 
	/// @return Move position list.
	Spread::List select(
		Map& map, const Skill& skill,
		SkillState& state,
		Hex* hex, sf::Vector2i pos
	);

	/// Randomly returns a position with the highest ranking.
	/// 
	/// @param list Position list.
	/// @param rank Ranking function.
	/// @param diff Difficulty.
	/// 
	/// @return Chosen position.
	std::optional<sf::Vector2i> get(
		const Spread::List& list,
		std::function<void(sf::Vector2i, int&)> rank,
		float diff
	);

	/// Returns a random empty spot around a tile.
	/// 
	/// @param map Map reference.
	/// @param pos Tile position.
	/// @param radius Search radius.
	/// @param team Origin team.
	/// 
	/// @return Random position.
	std::optional<sf::Vector2i> empty(
		Map& map,
		sf::Vector2i pos,
		size_t radius,
		Region::Team team
	);

	/// Generates a list of moves for a team.
	/// 
	/// @param map Map reference.
	/// @param team Controlled team.
	/// @param diff AI difficulty.
	/// 
	/// @return Move list.
	History::UniqList generate(Map& map, Region::Team team, float diff);
};
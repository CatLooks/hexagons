#pragma once
#include "skill_values.hpp"
#include "game/troop.hpp"

namespace Values {
	/// Texture maps for troops.
	/// @param id Troop type.
	extern const sf::IntRect troop_textures[Troop::Count];

	/// Localization requests for troops.
	/// @param id Troop type.
	extern const char* troop_names[Troop::Count];

	/// Troops available in the region shoop.
	extern const std::vector<Troop::Type> troop_shop;

	/// Troop health bar texture.
	extern const sf::IntRect hp_base;
	/// Poisoned health bar offset from normal bar.
	extern const sf::Vector2i poison_offset;

	/// Returns health bar texture based on entity state.
	/// 
	/// @param hp Current HP.
	/// @param max Maximum HP.
	/// @param poison Whether poison is applied.
	/// 
	/// @return Health bar texture.
	sf::IntRect hp_bar(int hp, int max, bool poison);

	/// Troop skills.
	extern const SkillArray troop_skills[Troop::Count];
};
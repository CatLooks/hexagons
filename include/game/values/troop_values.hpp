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

	/// Troop skills.
	extern const SkillArray troop_skills[Troop::Count];
};
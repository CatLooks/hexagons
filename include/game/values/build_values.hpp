#pragma once
#include "skill_values.hpp"
#include "game/build.hpp"

namespace Values {
	/// Texture maps for buildings.
	/// @param id Building type.
	extern const sf::IntRect build_textures[Build::Count];

	/// Localization requests for buildings.
	/// @param id Building type.
	extern const char* build_names[Build::Count];

	/// Building skills.
	extern const SkillArray build_skills[Build::Count];

	/// Buildings available in the region shop.
	extern const std::vector<Build::Type> build_shop;
};
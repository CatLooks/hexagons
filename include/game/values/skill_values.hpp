#pragma once
#include "shared.hpp"
#include "game/skill.hpp"
#include <vector>

namespace Values {
	/// Skills textures.
	extern const sf::IntRect skill_textures[Skills::Count];

	/// Skills names.
	extern const char* skill_names[Skills::Count];

	/// Action annotation icon textures.
	extern const sf::IntRect annotations[Skill::Count];
};
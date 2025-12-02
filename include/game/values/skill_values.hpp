#pragma once
#include "shared.hpp"
#include "game/skill.hpp"
#include <vector>

namespace Values {
	/// Entity skill data.
	struct SkillArray {
		/// Skill UI layout.
		enum Layout {
			None, /// Preview is hidden.
			L00,  /// 0 boxes on left, 0 boxes on right.
			L10,  /// 1 boxes on left, 0 boxes on right.
			L01,  /// 0 boxes on left, 1 boxes on right.
			L11,  /// 1 boxes on left, 1 boxes on right.
			L21,  /// 2 boxes on left, 1 boxes on right.
			L12,  /// 1 boxes on left, 2 boxes on right.
			L22,  /// 2 boxes on left, 2 boxes on right.
			Count
		};

		/// Skill layout.
		Layout layout;
		/// Skill description array.
		const SkillDesc* skills[4];
	};

	/// Skills textures.
	extern const sf::IntRect skill_textures[Skill::Count];

	/// Skills names.
	extern const char* skill_names[Skill::Count];

	/// Action annotation icon textures.
	extern const sf::IntRect annotations[SkillDesc::Count];
};
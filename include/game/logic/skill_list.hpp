#pragma once

// include dependencies
#include "skill_helper.hpp"
#include "game/values/troop_values.hpp"

/// List of all skills.
namespace SkillList {
	// entity buy
	extern const Skill buy_troop;
	extern const Skill buy_build;
	extern const Skill buy_troop_aim;
	extern const Skill buy_build_aim;

	// general troop / building skills
	extern const Skill withdraw;
	extern const Skill move;
	
	// attack skills
	extern const Skill attack_lumber;
	extern const Skill attack_spear;
	extern const Skill attack_archer;
	extern const Skill attack_baron;
	extern const Skill attack_knight;

	// status effect skills
	extern const Skill effect_defend;
	extern const Skill effect_range;
	extern const Skill effect_defense;
	extern const Skill effect_offense;

	// other special skills
	extern const Skill fruit;
	extern const Skill cut;
	extern const Skill tent;
	extern Skill heal1;
	extern Skill heal2;
	extern const Skill stun;
};
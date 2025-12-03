#pragma once

// include dependencies
#include "game/skill.hpp"
#include "game/map.hpp"

/// List of all skills.
namespace SkillList {
	// entity buy
	extern const SkillDesc buy_troop;
	extern const SkillDesc buy_build;
	extern const SkillDesc buy_troop_aim;
	extern const SkillDesc buy_build_aim;

	// general troop / building skills
	extern const SkillDesc withdraw;
	extern const SkillDesc move;
	
	// attack skills
	extern const SkillDesc attack_lumber;
	extern const SkillDesc attack_spear;
	extern const SkillDesc attack_archer;
	extern const SkillDesc attack_baron;
	extern const SkillDesc attack_knight;

	// status effect skills
	extern const SkillDesc effect_defend;
	extern const SkillDesc effect_range;
	extern const SkillDesc effect_defense;
	extern const SkillDesc effect_offense;

	// other special skills
	extern const SkillDesc fruit;
	extern const SkillDesc cut;
	extern const SkillDesc heal;
	extern const SkillDesc stun;
};
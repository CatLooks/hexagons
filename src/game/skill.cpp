#include "game/skill.hpp"

/// Skill resource label text path.
const char* Skills::withLabel[4] = {
	"dp.empty",
	"gp.cost.coins",
	"gp.cost.berry",
	"gp.cost.peach",
};

/// Returns the amount of a specified resource.
int SkillState::with(Skills::Resource resource) const {
	// fallback if no region selected
	if (!region) return 0;

	// select a resource
	switch (resource) {
		case Skills::Money: return region->money;
		case Skills::Berry: return region->berry;
		case Skills::Peach: return region->peach;
		default: return 0;
	};
};

namespace Skills {
	/// Checks whether a skill cooldown can be decreased.
	bool ticked(Type type, const std::vector<EffectType>& effects) {
		// check if list has an effect
		auto check = [&](EffectType eff) {
			for (EffectType e : effects)
				if (e == eff) return true;
			return false;
		};

		// check effect skills
		switch (type) {
			case RangeBoost: return !check(EffectType::RangeBoost);
			case DefenseBoost: return !check(EffectType::DefenseBoost);
			case OffenseBoost: return !check(EffectType::OffenseBoost);
		};
		return true;
	};
};

/// Returns region variable counters.
RegionVar SkillState::var() const {
	return region ? *region : RegionVar();
};
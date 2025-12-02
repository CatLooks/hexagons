#include "game/values/build_values.hpp"
#include "game/logic/skill_list.hpp"

namespace Values {
	// texture array
	const sf::IntRect build_textures[Build::Count] = {
		coords(0, 2), // town hall
		coords(1, 2), // farm
		coords(2, 2), // mine
		coords(3, 2), // tower
		coords(4, 2), // beacon
		coords(5, 2), // fort
	};

	// name array
	const char* build_names[Build::Count] = {
		"@!build.castle",
		"@!build.farm",
		"@!build.mine",
		"@!build.tower",
		"@!build.beacon",
		"@!build.fort",
	};

	// skill array
	const SkillArray build_skills[Build::Count] = {
		/* castle */ { SkillArray::L10, {
			& SkillList::heal
		} },
		/* farm   */ { SkillArray::L01, {
			& SkillList::withdraw
		} },
		/* mine   */ { SkillArray::L01, {
			& SkillList::withdraw
		} },
		/* tower  */ { SkillArray::L01, {
			& SkillList::withdraw
		} },
		/* beacon */ { SkillArray::L11, {
			& SkillList::stun,
			& SkillList::withdraw
		} },
		/* fort   */ { SkillArray::L11, {
			& SkillList::stun,
			& SkillList::withdraw
		} }
	};

	/// Buildings available in the region shop.
	const std::vector<Build::Type> build_shop = {
		Build::Farm, Build::Tower, Build::Beacon
	};
};
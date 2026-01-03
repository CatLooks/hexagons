#include "game/values/skill_values.hpp"

namespace Values {
	// skill textures
	const sf::IntRect skill_textures[Skills::Count] = {
		{}, // empty

		{}, // buy build
		{}, // buy troop

		coords(0, 1), // withdraw
		coords(6, 1), // move

		coords(1, 1), // lumberjack
		coords(2, 1), // spearman
		coords(3, 1), // archer
		coords(4, 1), // baron
		coords(5, 1), // knight

		coords(0, 2), // harvest
		coords(1, 2), // cut
		coords(3, 3), // tent
		coords(2, 2), // defend
		coords(3, 2), // range
		coords(4, 2), // defense
		coords(5, 2), // offense

		coords(4, 3), // heal 1
		coords(5, 3), // heal 2
		coords(1, 3), // stun
	};

	// skill names
	const char* skill_names[Skills::Count] = {
		"@!skill.invalid",

		"@!gp.buy_build",
		"@!gp.buy_troop",

		"@!skill.withdraw",
		"@!skill.move",

		"@!skill.attack.lumber",
		"@!skill.attack.spear",
		"@!skill.attack.archer",
		"@!skill.attack.baron",
		"@!skill.attack.knight",

		"@!skill.harvest",
		"@!skill.cut",
		"@!skill.tent",
		"@!skill.defend",
		"@!skill.range",
		"@!skill.defense",
		"@!skill.offense",

		"@!skill.heal_1",
		"@!skill.heal_2",
		"@!skill.stun",
	};

	/// Action annotation icon textures.
	const sf::IntRect annotations[Skill::Count] = {
		{},                 // none

		coords(8, 3, 0, 2), // peach
		coords(8, 3, 1, 2), // berry
		coords(8, 3, 2, 2), // aim
		coords(8, 3, 3, 2), // swap
		coords(8, 3, 0, 3), // manage
	};
};
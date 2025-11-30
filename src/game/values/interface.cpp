#include "game/values/interface.hpp"

namespace Values {
	/// Troop buying action texture.
	const sf::IntRect buy_build = coords(6, 3);
	/// Building buying action texture.
	const sf::IntRect buy_troop = coords(6, 2);

	/// Income arrow textures.
	const sf::IntRect income_arrow[3] = {
		coords(7, 0), // up
		coords(8, 0), // zero
		coords(9, 0), // down
	};

	/// Coin currency icon.
	const sf::IntRect coin_icon = coords(7, 1);
	/// Peach currency icon.
	const sf::IntRect peach_icon = coords(8, 1);
	/// Berry currency icon.
	const sf::IntRect berry_icon = coords(9, 1);

	/// Skills textures.
	const sf::IntRect skill_textures[static_cast<int>(SkillType::Count)] = {
		coords(9, 3, 3, 3), // empty

		coords(0, 1), // withdraw
		coords(6, 1), // move

		coords(1, 1), // lumberjack
		coords(2, 1), // spearman
		coords(3, 1), // archer
		coords(4, 1), // baron
		coords(5, 1), // knight

		coords(0, 2), // harvest
		coords(1, 2), // cut
		coords(2, 2), // defend
		coords(3, 2), // range
		coords(4, 2), // defense
		coords(5, 2), // offense

		coords(0, 3), // heal
		coords(1, 3), // stun
	};

	/// Skills names.
	const char* skill_names[static_cast<int>(SkillType::Count)] = {
		"@!skill.invalid",
		"@!skill.withdraw",
		"@!skill.move",

		"@!skill.attack.lumber",
		"@!skill.attack.spear",
		"@!skill.attack.archer",
		"@!skill.attack.baron",
		"@!skill.attack.knight",

		"@!skill.harvest",
		"@!skill.cut",
		"@!skill.defend",
		"@!skill.range",
		"@!skill.defense",
		"@!skill.offense",

		"@!skill.heal",
		"@!skill.stun",
	};

	/// Troop effect textures.
	const sf::IntRect effects[static_cast<int>(EffectType::Count)] = {
		coords(8, 3, 0, 0), // shield
		coords(8, 3, 1, 0), // range
		coords(8, 3, 2, 0), // defense
		coords(8, 3, 3, 0), // offense
		coords(8, 3, 0, 1), // poison
	};

	/// Action annotation icon textures.
	const sf::IntRect annotations[static_cast<int>(Annotation::Count)] = {
		coords(8, 3, 0, 2), // peach
		coords(8, 3, 1, 2), // berry
		coords(8, 3, 2, 2), // aim
		coords(8, 3, 3, 2), // swap
		coords(8, 3, 0, 3), // manage
	};

	/// Interface digit textures.
	const sf::IntRect digits[10] = {
		coords(9, 3, 0, 0),
		coords(9, 3, 1, 0),
		coords(9, 3, 2, 0),
		coords(9, 3, 3, 0),
		coords(9, 3, 0, 1),
		coords(9, 3, 1, 1),
		coords(9, 3, 2, 1),
		coords(9, 3, 3, 1),
		coords(9, 3, 0, 2),
		coords(9, 3, 1, 2),
	};

	/// Game panel text settings.
	const ui::TextSettings panel_text = {
		assets::font, 24, sf::Color::White,
		sf::Color::Black, 2
	};
};
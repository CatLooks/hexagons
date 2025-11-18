#include "game/values/troop_values.hpp"
#include "mathext.hpp"

namespace Values {
	// texture array
	const sf::IntRect troop_textures[Troop::Count] = {
		coords(0, 1), // farmer
		coords(1, 1), // lumberjack
		coords(2, 1), // spearman
		coords(3, 1), // archer
		coords(4, 1), // baron
		coords(5, 1), // knight
	};

	// name array
	const char* troop_names[Troop::Count] = {
		"@!troop.farmer",
		"@!troop.lumber",
		"@!troop.spear",
		"@!troop.archer",
		"@!troop.baron",
		"@!troop.knight",
	};

	// health bar base
	const sf::IntRect hp_base = sf::IntRect({ 399, 64 }, { 34, 5 });

	/// Poison health bar textures offset from normal bars.
	const sf::Vector2i poison_bar = { 64, 0 };

	/// Returns a texture map for troop health bar.
	sf::IntRect troop_hp(const Troop& troop) {
		// get bar fill values
		int now = troop.hp;
		int max = troop.hp; // @todo

		// rescale percentage
		if (max == 1) { now *= 6; max = 6; };
		if (max == 2) { now *= 3; max = 6; };
		if (max == 3) { now *= 2; max = 6; };
		TroopBar bar = TroopBar::UX;

		// x / 4
		if (max == 4) switch (now) {
			case 0: bar = TroopBar::Q0; break;
			case 1: bar = TroopBar::Q1; break;
			case 2: bar = TroopBar::Q2; break;
			case 3: bar = TroopBar::Q3; break;
			case 4: bar = TroopBar::Q4; break;
			default: break;
		}
		// x / 6
		else if (max == 6) switch (now) {
			case 0: bar = TroopBar::S0; break;
			case 1: bar = TroopBar::S1; break;
			case 2: bar = TroopBar::S2; break;
			case 3: bar = TroopBar::S3; break;
			case 4: bar = TroopBar::S4; break;
			case 5: bar = TroopBar::S5; break;
			case 6: bar = TroopBar::S6; break;
			default: break;
		};

		// return bar texture
		return hp_base + sf::Vector2i(0, hp_base.size.y * static_cast<int>(bar));
	};
};
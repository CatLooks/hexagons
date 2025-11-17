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

	// health bar array
	const sf::IntRect troop_hp[static_cast<size_t>(TroopBar::Count)] = {
		hp_base + sf::Vector2i(0, hp_base.size.y * 0), // 0 / 6
		hp_base + sf::Vector2i(0, hp_base.size.y * 1), // 1 / 6
		hp_base + sf::Vector2i(0, hp_base.size.y * 2), // 2 / 6
		hp_base + sf::Vector2i(0, hp_base.size.y * 3), // 3 / 6
		hp_base + sf::Vector2i(0, hp_base.size.y * 4), // 4 / 6
		hp_base + sf::Vector2i(0, hp_base.size.y * 5), // 5 / 6
		hp_base + sf::Vector2i(0, hp_base.size.y * 6), // 6 / 6
		hp_base + sf::Vector2i(0, hp_base.size.y * 7), // 1 / 4
		hp_base + sf::Vector2i(0, hp_base.size.y * 8), // 2 / 4
		hp_base + sf::Vector2i(0, hp_base.size.y * 9), // ?
	};

	/// Poison health bar textures offset from normal bars.
	const sf::Vector2i poison_bar = { 64, 0 };
};
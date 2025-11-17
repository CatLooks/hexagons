#pragma once
#include "shared.hpp"
#include "game/troop.hpp"

namespace Values {
	/// Texture maps for troops.
	/// @param id Troop type.
	extern const sf::IntRect troop_textures[Troop::Count];

	/// Localization requests for troops.
	/// @param id Troop type.
	extern const char* troop_names[Troop::Count];

	/// Troop health bar texture type.
	enum class TroopBar {
		S0, // 0 / 6 = 0%
		S1, // 1 / 6
		S2, // 2 / 6 = 1 / 3
		S3, // 3 / 6 = 1 / 2
		S4, // 4 / 6 = 2 / 3
		S5, // 5 / 6
		S6, // 6 / 6 = 100%
		Q1, // 1 / 4
		Q3, // 3 / 4
		UX, // invalid

		Count
	};

	/// Troop health bar textures.
	///
	/// @param id Bar fill amount.
	extern const sf::IntRect troop_hp[static_cast<size_t>(TroopBar::Count)];

	/// Poison health bar textures offset from normal bars.
	extern const sf::Vector2i poison_bar;
};
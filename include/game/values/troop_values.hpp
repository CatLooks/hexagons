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
		S0 = 0, // 0 / 6
		S1 = 1, // 1 / 6
		S2 = 2, // 2 / 6
		S3 = 3, // 3 / 6
		S4 = 4, // 4 / 6
		S5 = 5, // 5 / 6
		S6 = 6, // 6 / 6

		Q0 = S0, // 0 / 4
		Q1 = 7 , // 1 / 4
		Q2 = S3, // 2 / 4
		Q3 = 8 , // 3 / 4
		Q4 = S6, // 4 / 4

		UX, // invalid
		Count
	};

	/// Troop health bar base texture.
	extern const sf::IntRect hp_base;
	/// Poison health bar textures offset from normal bars.
	extern const sf::Vector2i poison_bar;

	/// Returns a texture map for troop health bar.
	/// 
	/// @param troop Troop reference.
	sf::IntRect troop_hp(const Troop& troop);
};
#pragma once
#include "shared.hpp"
#include "game/entity.hpp"
#include "mathext.hpp"

namespace Values {
	/// Troop health bar texture.
	extern const sf::IntRect hp_base;
	/// Poisoned health bar offset from normal bar.
	extern const sf::Vector2i poison_offset;

	/// Returns health bar texture based on entity state.
	/// 
	/// @param hp Current HP.
	/// @param max Maximum HP.
	/// @param poison Whether poison is applied.
	/// 
	/// @return Health bar texture.
	sf::IntRect hp_bar(int hp, int max, bool poison);

	/// Status effect names.
	extern const char* effect_names[static_cast<int>(EffectType::Count)];

	/// Shortened status effect names.
	extern const char* effect_names_short[static_cast<int>(EffectType::Count)];

	/// Status effect textures.
	extern const sf::IntRect effect_textures[static_cast<int>(EffectType::Count)];
};
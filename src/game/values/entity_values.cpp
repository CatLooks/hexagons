#include "game/values/entity_values.hpp"

namespace Values {
	// health bar base
	const sf::IntRect hp_base = sf::IntRect({ 463, 64 }, { 34, 5 });
	// poisoned health bar offset
	const sf::Vector2i poison_offset = { 0, 96 };

	// health bar offsets
	static const int hp_offset_0 = 0 ; // 0% fill
	static const int hp_offset_1 = 16; // x/1 fill
	static const int hp_offset_2 = 14; // x/2 fill
	static const int hp_offset_3 = 11; // x/3 fill
	static const int hp_offset_4 = 7 ; // x/4 fill
	static const int hp_offset_6 = 1 ; // x/6 fill
	static const int hp_offset_x = 17; // bad fill

	/// Returns health bar texture based on entity state.
	sf::IntRect hp_bar(int hp, int max, bool poison) {
		// get bar texture index
		int idx = [=]() {
			// discard invalid HP
			if (hp < 0 || hp > max) return hp_offset_x;
			// check for 0% fill
			if (hp == 0) return hp_offset_0;
			// map HP / max fraction
			switch (max) {
				case 1: return hp_offset_1 + hp - 1;
				case 2: return hp_offset_2 + hp - 1;
				case 3: return hp_offset_3 + hp - 1;
				case 4: return hp_offset_4 + hp - 1;
				case 6: return hp_offset_6 + hp - 1;
				default: return hp_offset_x;
			};
		}();

		// remap index into texture
		return hp_base
			+ sf::Vector2i(0, hp_base.size.y * idx)
			+ (poison ? poison_offset : sf::Vector2i());
	};

	/// Status effect names.
	const char* effect_names[static_cast<int>(EffectType::Count)] = {
		"@!effect.shield",
		"@!effect.range",
		"@!effect.defense",
		"@!effect.offense",
		"@!effect.poison",
		"@!effect.stun",
		"@!effect.enchant"
	};

	/// Shortened status effect names.
	const char* effect_names_short[static_cast<int>(EffectType::Count)] = {
		"effect.short.shield",
		"effect.short.range",
		"effect.short.defense",
		"effect.short.offense",
		"effect.short.poison",
		"effect.short.stun",
		"effect.short.enchant"
	};

	/// Status effect textures.
	const sf::IntRect effect_textures[static_cast<int>(EffectType::Count)] = {
		coords(8, 3, 0, 0), // shield
		coords(8, 3, 1, 0), // range
		coords(8, 3, 2, 0), // defense
		coords(8, 3, 3, 0), // offense
		coords(8, 3, 0, 1), // poison
		coords(8, 3, 1, 1), // stunned
		coords(8, 3, 2, 1), // enchant
	};
};
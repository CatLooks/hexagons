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

	// skill array
	const SkillArray troop_skills[Troop::Count] = {
		/* farmer */ { SkillArray::L12, { SkillType::Move, SkillType::Harvest     , SkillType::Withdraw    , SkillType::Empty    } },
		/* lumber */ { SkillArray::L22, { SkillType::Move, SkillType::AttackLumber, SkillType::TreeCut     , SkillType::Withdraw } },
		/* spear  */ { SkillArray::L22, { SkillType::Move, SkillType::AttackSpear , SkillType::Shield      , SkillType::Withdraw } },
		/* archer */ { SkillArray::L22, { SkillType::Move, SkillType::AttackArcher, SkillType::RangeBoost  , SkillType::Withdraw } },
		/* baron  */ { SkillArray::L22, { SkillType::Move, SkillType::AttackBaron , SkillType::DefenseBoost, SkillType::Withdraw } },
		/* knight */ { SkillArray::L22, { SkillType::Move, SkillType::AttackKnight, SkillType::OffenseBoost, SkillType::Withdraw } },
	};

	// max hp
	const int troop_hp[Troop::Count] = {
		1, 2, 3, 2, 4, 6
	};
};
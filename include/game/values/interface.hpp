#pragma once
#include "shared.hpp"
#include "ui/text.hpp"
#include "assets.hpp"

/// Skill enumeration.
enum class SkillType {
	Withdraw,     /// Withdraw.

	AttackLumber, /// Lumberjack attack.
	AttackSpear,  /// Spearman attack.
	AttackArcher, /// Archer attack.
	AttackBaron,  /// Baron attack.
	AttackKnight, /// Knight attack.

	Harvest,      /// (Farmer) Fruit harvest.
	TreeCut,      /// (Lumberjack) Plant cut down.
	Shield,       /// (Spearman) Spearman shield.
	RangeBoost,   /// (Archer) Range boost.
	DefenseBoost, /// (Baron) Defense boost.
	AttackBoost,  /// (Knight) Attack boost.

	Heal,         /// (Castle) Heal troops.
	Stun,         /// (Beacon) Stun troops.
	Count
};

/// Effect enumeration.
enum class EffectType {
	Shielded,     /// Shield for spearman.
	RangeBoost,   /// Range boost for archer.
	DefenseBoost, /// Defense boost for baron.
	AttackBoost,  /// Attack boost for knight.
	Count
};

namespace Values {
	/// Troop buying action texture.
	extern const sf::IntRect buy_build;
	/// Building buying action texture.
	extern const sf::IntRect buy_troop;

	/// Income arrow textures.
	extern const sf::IntRect income_arrow[3];

	/// Troop skills textures.
	extern const sf::IntRect skills[static_cast<int>(SkillType::Count)];

	/// Troop effect textures.
	extern const sf::IntRect effects[static_cast<int>(EffectType::Count)];

	/// Action annotation icon.
	enum class Annotation {
		Peach, /// Action costs peaches.
		Berry, /// Action costs berries.
		Aim,   /// Action needs to be aimed.
		Swap,  /// Action is an cyclic choice.
		Count
	};

	/// Action annotation icon textures.
	extern const sf::IntRect annotations[static_cast<int>(Annotation::Count)];
	/// Interface digit textures.
	extern const sf::IntRect digits[10];

	/// Game panel text settings.
	extern const ui::TextSettings panel_text;
};
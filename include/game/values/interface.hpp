#pragma once
#include "shared.hpp"
#include "ui/text.hpp"
#include "assets.hpp"

/// Skill enumeration.
enum class SkillType {
	Empty = 0,    /// Empty skill.

	Withdraw,     /// Withdraw.
	Move,         /// Move.

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
	OffenseBoost, /// (Knight) Attack boost.

	Heal,         /// (Castle) Heal troops.
	Stun,         /// (Beacon) Stun troops.
	Count
};

/// Effect enumeration.
enum class EffectType {
	Shielded,     /// Shield for spearman.
	RangeBoost,   /// Range boost for archer.
	DefenseBoost, /// Defense boost for baron.
	OffenseBoost, /// Attack boost for knight.
	Poisoned,     /// Poisoned by archer.
	Count
};

namespace Values {
	/// Entity skill data.
	struct SkillArray {
		/// Skill UI layout.
		enum Layout {
			None, /// Preview is hidden.
			L00,  /// 0 boxes on left, 0 boxes on right.
			L10,  /// 1 boxes on left, 0 boxes on right.
			L01,  /// 0 boxes on left, 1 boxes on right.
			L11,  /// 1 boxes on left, 1 boxes on right.
			L21,  /// 2 boxes on left, 1 boxes on right.
			L12,  /// 1 boxes on left, 2 boxes on right.
			L22,  /// 2 boxes on left, 2 boxes on right.
			Count
		};

		/// Skill layout.
		Layout layout = None;
		/// Skill array.
		SkillType skills[4] {};
	};

	/// Troop buying action texture.
	extern const sf::IntRect buy_build;
	/// Building buying action texture.
	extern const sf::IntRect buy_troop;

	/// Income arrow textures.
	extern const sf::IntRect income_arrow[3];

	/// Coin currency icon.
	extern const sf::IntRect coin_icon;
	/// Peach currency icon.
	extern const sf::IntRect peach_icon;
	/// Berry currency icon.
	extern const sf::IntRect berry_icon;

	/// Skills textures.
	extern const sf::IntRect skill_textures[static_cast<int>(SkillType::Count)];

	/// Skills names.
	extern const char* skill_names[static_cast<int>(SkillType::Count)];

	/// Troop effect textures.
	extern const sf::IntRect effects[static_cast<int>(EffectType::Count)];

	/// Action annotation icon.
	enum class Annotation {
		Peach,  /// Action costs peaches.
		Berry,  /// Action costs berries.
		Aim,    /// Action needs to be aimed.
		Swap,   /// Action is an cyclic choice.
		Manage, /// Action manages other entity.
		Count
	};

	/// Action annotation icon textures.
	extern const sf::IntRect annotations[static_cast<int>(Annotation::Count)];
	/// Interface digit textures.
	extern const sf::IntRect digits[10];

	/// Game panel text settings.
	extern const ui::TextSettings panel_text;
};
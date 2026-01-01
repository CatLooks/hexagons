#pragma once

/// Skill enumeration container.
namespace Skills {
	/// Skill enumeration.
	enum Type {
		Empty = 0,    /// Empty skill.

		BuyBuild,     /// Buy a building.
		BuyTroop,     /// Buy a troop.

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
};
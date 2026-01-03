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

		TentSet,      /// (Spearman) Tent setup.
		Shield,       /// (Spearman) Spearman shield.

		RangeBoost,   /// (Archer) Range boost.
		DefenseBoost, /// (Baron) Defense boost.
		OffenseBoost, /// (Knight) Attack boost.

		Heal_1,       /// (Tent) 1 HP troop heal.
		Heal_2,       /// (Tent) 2 HP troop heal.
		Stun,         /// (Beacon) Stun troops.
		Count
	};
};
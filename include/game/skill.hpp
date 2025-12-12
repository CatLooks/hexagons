#pragma once

// include dependencies
#include <variant>
#include "spread.hpp"

/// Skill enumeration container.
namespace Skills {
	/// Skill enumeration.
	enum Type {
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
};

/// Skill description object.
struct Skill {
	/// Skill type.
	Skills::Type type = Skills::Empty;

	/// Action annotation icon.
	enum Annotation {
		None,   /// No annotation.
		Peach,  /// Action costs peaches.
		Berry,  /// Action costs berries.
		Aim,    /// Action needs to be aimed.
		Swap,   /// Action is an cyclic choice.
		Manage, /// Action manages other entity.
		Count
	};

	/// Skill annotation.
	Annotation annotation = None;

	/// Selection tile spreader generator.
	///
	/// @param tile Map selection origin.
	/// @param idx Map selection index.
	using Selection = std::function<Spread(const HexRef& tile, size_t idx)>;

	/// Selection tile spreader generator.
	Selection select = [](const HexRef&, size_t) { return Spread(); };

	/// Selection tile spreader radius.
	size_t radius = 0;

	/// Skill action function type.
	///
	/// @param map Map reference.
	/// @param prev Action origin.
	/// @param next Action destination.
	using Action = std::function<void(Map& map, const HexRef& prev, const HexRef& next)>;

	/// Skill action.
	Action action = [](Map&, const HexRef&, const HexRef&) {};

	/// Skill format type.
	enum Format {
		Self,      /// Double-click on skill button.
		SingleAim, /// Click on skill button, then on selected tile.
		DoubleAim, /// Click on skill button, then twice on selected tiles.
	};

	/// Skill interaction format.
	Format format = SingleAim;

	/// Whether to select target tile after skill action.
	bool reselect = false;
};
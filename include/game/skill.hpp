#pragma once

// include dependencies
#include <variant>
#include "spread.hpp"
#include "region.hpp"

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

	/// Skill used resource type.
	enum Resource {
		None,  /// Free skill.
		Berry, /// Skill requires berries.
		Peach, /// Skill requires peaches.
	};

	/// Skill resource label text path.
	extern const char* withLabel[3];
};

/// Data required for skill calculations.
struct SkillState {
	int build     = 0; /// Current building index in shop.
	int troop     = 0; /// Current troop index in shop.
	Region* region {}; /// Currently selected region.

	/// Returns the amount of a specified resource.
	///
	/// @param resource Resource type.
	///
	/// @return Amount of the resource stored in state.
	int with(Skills::Resource resource) const;
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

	/// Resource required for skill.
	Skills::Resource resource = Skills::None;
	/// Skill cost.
	int cost = 0;

	/// Skill action condition check type.
	///
	/// @param state Skill state.
	/// @param tile Skill target tile.
	using Condition = std::function<bool(const SkillState& state, const HexRef& tile)>;

	/// Skill execution condition.
	///
	/// By default, uses built-in skill cost fields.
	Condition condition = [this](const SkillState& state, const HexRef&) {
		return state.with(resource) >= cost;
	};

	/// Selection tile spreader generator.
	///
	/// @param state Skill state.
	/// @param tile Map selection origin.
	/// @param idx Map selection index.
	using Selection = std::function<Spread(const SkillState& state, const HexRef& tile, size_t idx)>;

	/// Selection tile spreader generator.
	///
	/// By default, does not select any tiles.
	Selection select = [](const SkillState&, const HexRef&, size_t) { return Spread(); };

	/// Selection tile spreader radius.
	size_t radius = 0;

	/// Skill action function type.
	///
	/// @param state Skill state.
	/// @param map Map reference.
	/// @param prev Action origin.
	/// @param next Action destination.
	using Action = std::function<void(const SkillState& state, Map& map, const HexRef& prev, const HexRef& next)>;

	/// Skill action.
	///
	/// By default, does nothing.
	Action action = [](const SkillState&, Map&, const HexRef&, const HexRef&) {};

	/// Skill format type.
	enum Format {
		Self,      /// Double-click on skill button.
		SingleAim, /// Click on skill button, then on selected tile.
		DoubleAim, /// Click on skill button, then twice on selected tiles.
	};

	/// Skill interaction format.
	Format format = SingleAim;

	/// Whether to select target tile after skill action.
	///
	/// Disabled by default.
	bool reselect = false;
};
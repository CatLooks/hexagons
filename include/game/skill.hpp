#pragma once

// include dependencies
#include <variant>
#include "spread.hpp"
#include "region.hpp"
#include "moves/move.hpp"
#include "logic/skill_types.hpp"
#include "logic/effect_types.hpp"

/// Skill enumeration container.
namespace Skills {
	/// Skill used resource type.
	enum Resource {
		None,  /// Free skill.
		Money, /// Skill requires money.
		Berry, /// Skill requires berries.
		Peach, /// Skill requires peaches.
	};

	/// Skill resource label text path.
	extern const char* withLabel[4];

	/// Checks whether a skill cooldown can be decreased.
	///
	/// @param type Skill type.
	/// @param effects Entity effect list.
	bool ticked(Type type, const std::vector<EffectType>& effects);
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

	/// Returns region variable counters.
	RegionVar var() const;
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
		Swap,   /// Action is a cyclic choice.
		Manage, /// Action manages other entity.
		Count
	};

	/// Skill annotation.
	Annotation annotation = None;

	/// Resource required for skill.
	Skills::Resource resource = Skills::None;

	/// Skill cost generator.
	///
	/// @param state Skill state.
	using Cost = std::function<int(const SkillState& state)>;

	/// Skill cost.
	Cost cost = [](const SkillState&) { return 0; };

	/// Skill action condition check type.
	///
	/// @param state Skill state.
	/// @param map Map reference.
	/// @param tile Skill target tile.
	using Condition = std::function<bool
		(const SkillState& state, Map& map, const HexRef& tile)>;

	/// Skill execution condition.
	///
	/// By default, uses built-in skill cost fields.
	Condition condition = [this](const SkillState& state, Map&, const HexRef&)
		{ return state.with(resource) >= cost(state); };

	/// Selection tile spreader generator.
	///
	/// @param state Skill state.
	/// @param tile Map selection origin.
	/// @param idx Map selection index.
	using Selection = std::function<Spread
		(const SkillState& state, const HexRef& tile, size_t idx)>;

	/// Selection tile spreader generator.
	///
	/// By default, does not select any tiles.
	Selection select = [](const SkillState&, const HexRef&, size_t idx)
	{
		return Spread {
			.effect = [=](const Spread::Tile& tile)
				{ tile.hex->selected = idx; }
		};
	};

	/// Selection tile spreader radius.
	size_t radius = 0;

	/// Skill action function type.
	///
	/// @param state Skill state.
	/// @param map Map reference.
	/// @param prev Action origin.
	/// @param next Action destination.
	/// 
	/// @return Action as a reversible move object (no action if `null`).
	using Action = std::function<Move*
		(const SkillState& state, Map& map, const HexRef& prev, const HexRef& next)>;

	/// Skill action.
	///
	/// By default, does nothing.
	Action action = [](const SkillState&, Map&, const HexRef&, const HexRef&)
		{ return nullptr; };

	/// Skill format type.
	enum Format {
		Self,      /// Double-click on skill button.
		SingleAim, /// Click on skill button, then on selected tile.
		DoubleAim, /// Click on skill button, then twice on selected tiles.
	};

	/// Skill interaction format.
	Format format = SingleAim;

	/// Skill cooldown (in turns).
	uint8_t cooldown = 1;

	/// Whether to select target tile after skill action.
	///
	/// Disabled by default.
	bool reselect = false;
};
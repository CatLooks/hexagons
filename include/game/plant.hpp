#pragma once

// include dependencies
#include "entity.hpp"
#include "region.hpp"

/// Plant object.
/// Stores a state of a plant.
struct Plant : Entity {
	/// Plant type.
	enum Type {
		Bush,    /// Bush.
		Berry,   /// Bush with berries.
		Sapling, /// Peach sapling.
		Tree,    /// Peach tree.
		Peach,   /// Peach tree with peaches.
		Pine,    /// Pine tree.
		Grave,   /// Grave.

		Count    /// Plant count.
	} type = Bush;

	/// Plant stage timer.
	/// Increases when plant fails stage advancement.
	uint8_t stage = 0;
	/// Spread timer.
	/// Increases when plant fails to spread.
	uint8_t spread = 0;
	/// Whether the plant has just been spawned.
	bool fresh = true;

	/// Rolls a stage advancement.
	bool stage_roll();
	/// Rolls a plant spread.
	bool spread_roll();

	/// Checks whether a plant is harvestable.
	bool harvestable() const;
	/// Harvests the plant.
	/// 
	/// @return Harvested resources.
	RegionRes harvest();

	/// Ticks entity state.
	void tick() override;
};
#pragma once

// include dependencies
#include "move.hpp"
#include "game/spread.hpp"

namespace Moves {
	/// Checks if a tile contains a harvestable plant.
	extern const Spread::Check harvestablePlant;

	/// Plant modification move.
	struct PlantMod : Move {
		sf::Vector2i mid; /// Modification center.
		size_t    radius; /// Modification radius.
		RegionRes  a_res; /// Harvested resources.

		/// Modification targets.
		std::vector<Plant> a_target;

		/// Constructs a plant modification move.
		/// 
		/// @param mid Modification center.
		/// @param radius Modification radius.
		PlantMod(sf::Vector2i mid, size_t radius);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};
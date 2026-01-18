#pragma once

// include dependencies
#include "move.hpp"
#include "game/spread.hpp"

namespace Moves {
	/// Generates a troop heal cost checker.
	/// 
	/// @param heal Heal amount.
	/// @param team Heal origin team.
	/// @param cost Heal cost pointer.
	/// 
	/// @return Cost check spreader.
	Spread troopHealCost(int heal, Region::Team team, int* cost);

	/// Troop healing move.
	struct TroopHeal : Move {
		size_t     radius; /// Heal radius.
		sf::Vector2i  mid; /// Heal center.
		int          heal; /// Heal amount.
		Region::Team team; /// Heal origin team.
		Spread     spread; /// Heal spreader.

		/// Heal target info.
		struct Target {
			sf::Vector2i pos; /// Target position.
			int    hp_before; /// HP before heal.
		};

		/// Heal targets.
		std::vector<Target> a_target;
		/// Heal cost (in berries).
		int a_cost;

		/// Constructs a troop heal move.
		/// 
		/// @param mid Heal center.
		/// @param radius Heal radius.
		/// @param heal Heal amount.
		/// @param team Heal origin team.
		TroopHeal(sf::Vector2i mid, size_t radius, int heal, Region::Team team);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};
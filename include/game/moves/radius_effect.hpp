#pragma once

// include dependencies
#include "move.hpp"
#include "game/spread.hpp"

namespace Moves {
	/// AoE effect application move.
	struct RadiusEffect : Move {
		EffectType effect; /// Applied effect.
		sf::Vector2i  mid; /// AoE middle.
		size_t     radius; /// AoE radius.
		Region::Team team; /// Effect origin team.
		Spread     spread; /// Target list populator.

		/// Effect target.
		struct Target {
			sf::Vector2i pos; /// Target position.
			bool      before; /// Whether the effect was applied before.
		};

		/// List of affected targets.
		std::vector<Target> a_target;

		/// Constructs an AoE effect move.
		/// 
		/// @param mid AoE center.
		/// @param radius AoE radius.
		/// @param effect Applied effect.
		/// @param team Origin team.
		RadiusEffect(sf::Vector2i mid, size_t radius, EffectType effect, Region::Team team);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};
#include "game/funcs.hpp"

namespace spreadf {
	/// Creates a same team spread check.
	Spread::Check sameTeam(Region::Team team) {
		return [team](Spread::Tile& tile) {
			return tile.hex->team == team;
		};
	};

	/// Creates a region audit spread effect.
	Spread::Effect regionAudit(const Regions::Ref& ref) {
		return [&ref](Spread::Tile& tile) {
			// audit hex contents
			ref->tiles++;
			ref->income++;

			// attach region to tile
			tile.hex->region = ref;
		};
	};

	/// Creates a region recalculation spreader.
	Spread regionCalculation(Region::Team team, const Regions::Ref& ref) {
		return Spread {
			.hop = sameTeam(team),
			.effect = regionAudit(ref),
			.imm = true
		};
	};
};
#include "game/logic/skill_helper.hpp"

namespace skillf {
	/// Generates tile selector spreader effect function.
	Spread::Effect selectTile(size_t idx) {
		return [=](Spread::Tile& tile) {
			tile.hex->selected = idx;
		};
	};

	/// Allows spreading only to solid tiles.
	const Spread::Check solidHop = [](const Spread::Tile& tile) {
		return tile.hex->solid();
	};
};
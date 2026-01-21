#include "game/bot_ai.hpp"

namespace ai {
	/// Generates a list of moves for a team.
	History::UniqList generate(Map& map, Region::Team team, float diff) {
		// generate move list for each region
		Regions::foreach(&map, [=](Region& reg, sf::Vector2i pos) {
			// ignore if not target region
			if (reg.team != team) return;

			
		});

		// return empty list lol
		return {};
	};
};
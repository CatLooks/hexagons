#include "game/moves/troop_moves.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs troop movement move.
	TroopMove::TroopMove(sf::Vector2i dest)
		: dest(dest), state{ Region::Unclaimed, {} } {};

	/// Applies the move.
	void TroopMove::onApply(Map* map) {
		// get tiles
		auto from = map->atref(skill_pos);
		auto to = map->atref(dest);
		if (!(from.hex && to.hex)) return;

		// store destination state
		state.team = to.hex->team;
		if (to.hex->troop) state.entity = *to.hex->troop;
		else if (to.hex->build) state.entity = *to.hex->build;
		else if (to.hex->plant) state.entity = *to.hex->plant;

		// remove entities at destination
		map->removeEntity(to.hex);

		// repaint destination tile
		Regions::Ref prev = to.hex->region();
		if (prev != from.hex->region()) {
			to.hex->team = from.hex->team;
			to.hex->join(from.hex->region());

			// update regions
			map->updateRegions(to, prev);
		};

		// move troop
		to.hex->troop = std::move(from.hex->troop);
		from.hex->troop = {};
	};

	/// Reverts the move.
	void TroopMove::onRevert(Map* map) {
		// get tiles
		auto to = map->atref(skill_pos);
		auto from = map->atref(dest);
		if (!(from.hex && to.hex)) return;

		// move troop
		to.hex->troop = std::move(from.hex->troop);
		from.hex->troop = {};

		// restore previous region
		Regions::Ref prev = from.hex->region();
		if (from.hex->team != state.team) {
			// check for the same team region nearby
			Regions::Ref reg;
			for (int i = 0; i < 6; i++) {
				// get neighbor tile
				auto* hex = map->at(map->neighbor(from.pos, static_cast<Map::nbi_t>(i)));
				if (!hex) return;

				// copy its region if has the same team
				if (hex->team == state.team) {
					reg = hex->region();
					break;
				};
			};

			// repaint tile region back
			from.hex->team = state.team;
			from.hex->join(reg ? reg : map->regions.create({ .team = state.team }));

			// update regions
			map->updateRegions(from, prev);
		};

		// place back previous entity
		if (auto* troop = std::get_if<Troop>(&state.entity))
			map->setTroop(*troop);
		if (auto* build = std::get_if<Build>(&state.entity))
			map->setBuild(*build);
		if (auto* plant = std::get_if<Plant>(&state.entity))
			map->setPlant(*plant);
	};

	/// Returns tile to select after moving.
	std::optional<sf::Vector2i> TroopMove::applyCursor() {
		return dest;
	};

	/// Returns tile to select after returning.
	std::optional<sf::Vector2i> TroopMove::revertCursor() {
		return skill_pos;
	};
};
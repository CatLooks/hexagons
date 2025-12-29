#include "game/moves/troop_move.hpp"
#include "game/values/hex_values.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs troop movement move.
	TroopMove::TroopMove(sf::Vector2i dest)
		: dest(dest), a_state{ Region::Unclaimed, {} } {};

	/// Applies the move.
	void TroopMove::onApply(Map* map) {
		// get tiles
		auto from = map->atref(skill_pos);
		auto to = map->atref(dest);
		if (!(from.hex && to.hex)) return;

		// store region state
		a_state.team = to.hex->team;
		a_state.resources = static_cast<RegionRes>(*to.hex->region());

		// store entity state
		if (to.hex->troop) a_state.entity = *to.hex->troop;
		else if (to.hex->build) a_state.entity = *to.hex->build;
		else if (to.hex->plant) a_state.entity = *to.hex->plant;

		// remove entities at destination
		map->removeEntity(to.hex);

		// repaint destination tile
		Regions::Ref prev = to.hex->region();
		if (prev != from.hex->region()) {
			to.hex->team = from.hex->team;
			to.hex->join(from.hex->region());

			// update regions
			a_state.split = map->updateRegions(to, prev, {});
		};

		// move troop
		to.hex->troop = std::move(from.hex->troop);
		to.hex->troop->pos = to.pos;
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
		to.hex->troop->pos = to.pos;
		from.hex->troop = {};

		// restore previous region
		Regions::Ref prev = from.hex->region();
		if (from.hex->team != a_state.team) {
			// check for the same team region nearby
			Regions::Ref reg;
			for (int i = 0; i < 6; i++) {
				// get neighbor tile
				auto* hex = map->at(map->neighbor(from.pos, static_cast<Map::nbi_t>(i)));
				if (!hex) continue;

				// copy its region if has the same team
				if (hex->team == a_state.team) {
					reg = hex->region();
					break;
				};
			};

			// reconstruct previous region
			if (!reg) {
				reg = map->regions.create({ .team = a_state.team });
				reg->add(a_state.resources);
			};

			// repaint tile region back
			from.hex->team = a_state.team;
			from.hex->join(reg);

			// update regions
			map->updateRegions(from, prev, a_state.split);
		};

		// place back previous entity
		if (auto* troop = std::get_if<Troop>(&a_state.entity))
			map->setTroop(*troop);
		if (auto* build = std::get_if<Build>(&a_state.entity))
			map->setBuild(*build);
		if (auto* plant = std::get_if<Plant>(&a_state.entity))
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

	/// Emits move section info.
	void TroopMove::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->extra("dp.move.troop_move.to");
		section->line("dp.move.override");
		section->line("dp.move.troop_move.region");
		section->line("dp.move.troop_move.splits");

		// add arguments
		list["pos"] = ext::str_vec(dest);
		list["team"] = Values::hex_names[a_state.team];
		list["res"] = std::format(
			"{}/{}/{}",
			a_state.resources.money,
			a_state.resources.berry,
			a_state.resources.peach
		);
		list["entity"] = str_ent(&a_state.entity);
		list["skill_name"] = "@!dp.move.name.troop_move";

		// split data
		std::string splits;
		for (const auto& split : a_state.split) {
			if (!splits.empty()) splits.append(" + ");
			splits.append(std::format("{}/{}/{}", split.money, split.berry, split.peach));
		};
		list["splits"] = splits.empty() ? "@!dp.empty" : splits;
	};
};
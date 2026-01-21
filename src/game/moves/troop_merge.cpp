#include "game/moves/troop_merge.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs a troop merge move.
	TroopMerge::TroopMerge(sf::Vector2i dest)
		: dest(dest), a_prev{} {};

	/// Applies the move.
	void TroopMerge::onApply(Map* map) {
		// get moved troop tile
		Hex* moved = map->at(skill_pos);
		if (!moved || !moved->troop) return;

		// get target tile
		Hex* hex = map->at(dest);
		if (!hex || !hex->troop) return;

		// get merged troop type
		Troop::Type merge = logic::troop_merge[moved->troop->type][hex->troop->type];
		if (merge >= Troop::Count) return;

		// store previous troop states
		a_prev[0] = *moved->troop;
		a_prev[1] = *hex->troop;
		
		// override target troop with merged troop
		Troop troop = *hex->troop;
		troop.type = merge;
		{
			// add hitpoints from both troops
			troop.hp = std::min(moved->troop->hp + hex->troop->hp + 1, troop.max_hp());

			// reset timers
			for (int i = 0; i < 4; i++)
				troop.timers[i] = 0;

			// set withdraw cooldown to 1
			troop.add_cooldown(Skills::Withdraw, 1);
		};
		map->setTroop(troop);

		// delete moved troop
		map->removeEntity(moved);
	};

	/// Reverts the move.
	void TroopMerge::onRevert(Map* map) {
		// get tiles
		Hex* a = map->at(skill_pos);
		Hex* b = map->at(dest);
		if (!a || !b) return;

		// restore previous troop states
		map->setTroop(a_prev[0]);
		map->setTroop(a_prev[1]);
	};

	/// Returns tile to select after moving.
	std::optional<sf::Vector2i> TroopMerge::applyCursor() {
		return dest;
	};

	/// Returns tile to select after returning.
	std::optional<sf::Vector2i> TroopMerge::revertCursor() {
		return skill_pos;
	};

	/// Emits move section info.
	void TroopMerge::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.troop_merge.a");
		section->line("dp.move.troop_merge.b");

		// add arguments
		EntState a = a_prev[0]; list["a"] = str_ent(&a);
		EntState b = a_prev[1]; list["b"] = str_ent(&b);
		list["skill_name"] = "@!dp.move.name.troop_merge";
	};
};
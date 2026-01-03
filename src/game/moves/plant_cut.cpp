#include "game/moves/plant_cut.hpp"
#include "game/map.hpp"

#include "game/values/plant_values.hpp"

namespace Moves {
	/// Constructs a plant cut move.
	PlantCut::PlantCut(sf::Vector2i pos) : pos(pos) {};

	/// Applies the move.
	void PlantCut::onApply(Map* map) {
		// get tiles
		Hex* from = map->at(skill_pos);
		if (!from) return;
		Hex* hex = map->at(pos);
		if (!(hex && hex->plant)) return;

		// store plant state
		a_state = *hex->plant;

		// remove the plant
		map->removeEntity(hex);

		// grant plant cut bonus
		if (from->region())
			from->region()->money += logic::plant_bonus[a_state.type];
	};

	/// Reverts the move.
	void PlantCut::onRevert(Map* map) {
		// get tiles
		Hex* from = map->at(skill_pos);
		if (!from) return;
		Hex* hex = map->at(pos);
		if (!hex) return;

		// restore the plant
		map->setPlant(a_state);

		// remove plant cut bonus
		if (from->region())
			from->region()->money -= logic::plant_bonus[a_state.type];
	};

	/// Emits move section info.
	void PlantCut::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.plant_cut");

		// add arguments
		list["to"] = ext::str_vec(pos);
		list["plant"] = Values::plant_names[a_state.type];
		list["bonus"] = ext::str_int(logic::plant_bonus[a_state.type]);
		list["skill_name"] = "@!dp.move.name.plant_cut";
	};
};
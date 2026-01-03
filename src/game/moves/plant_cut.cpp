#include "game/moves/plant_cut.hpp"
#include "game/map.hpp"

#include "game/values/plant_values.hpp"

namespace Moves {
	/// Constructs a plant cut move.
	PlantCut::PlantCut(sf::Vector2i origin, sf::Vector2i pos)
		: from(origin), pos(pos) {};

	/// Applies the move.
	void PlantCut::onApply(Map* map) {
		// get tiles
		Hex* org = map->at(from);
		if (!org) return;
		Hex* hex = map->at(pos);
		if (!(hex && hex->plant)) return;

		// store plant state
		a_state = *hex->plant;

		// remove the plant
		map->removeEntity(hex);

		// grant plant cut bonus
		if (org->region())
			org->region()->money += logic::plant_bonus[a_state.type];
	};

	/// Reverts the move.
	void PlantCut::onRevert(Map* map) {
		// get tiles
		Hex* org = map->at(from);
		if (!org) return;
		Hex* hex = map->at(pos);
		if (!hex) return;

		// restore the plant
		map->setPlant(a_state);

		// remove plant cut bonus
		if (org->region())
			org->region()->money -= logic::plant_bonus[a_state.type];
	};

	/// Emits move section info.
	void PlantCut::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.plant_cut.info");
		section->line("dp.move.plant_cut.vector");

		// add arguments
		list["from"] = ext::str_vec(from);
		list["to"] = ext::str_vec(pos);
		list["plant"] = Values::plant_names[a_state.type];
		list["bonus"] = ext::str_int(logic::plant_bonus[a_state.type]);
		list["skill_name"] = "@!dp.move.name.plant_cut";
	};
};
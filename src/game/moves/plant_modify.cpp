#include "game/moves/plant_modify.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Checks if a tile contains a harvestable plant.
	const Spread::Check harvestablePlant = [](const Spread::Tile& tile) {
		return tile.hex->plant && tile.hex->plant->harvestable();
	};

	/// Constructs a plant modification move.
	PlantMod::PlantMod(sf::Vector2i mid, size_t radius)
		: mid(mid), radius(radius) {};

	/// Applies the move.
	void PlantMod::onApply(Map* map) {
		// get origin region
		Hex* hex = map->at(mid);
		if (!hex) return;

		// harvest all plants in range
		a_res = {};
		a_target.clear();
		Spread spread = {
			.pass = harvestablePlant,
			.effect = [=](const Spread::Tile& tile) {
				Plant& plant = *tile.hex->plant;

				// store plant state
				a_target.push_back(plant);

				// harvest resources from the plant
				a_res.add(plant.harvest());
			}
		};
		spread.apply(*map, mid, radius);

		// store harvested resources
		if (hex->region())
			hex->region()->add(a_res);
	};

	/// Reverts the move.
	void PlantMod::onRevert(Map* map) {
		// get origin region
		Hex* hex = map->at(mid);
		if (!hex) return;

		// revert plant states
		for (const Plant& plant : a_target)
			map->setPlant(plant);

		// subtract harvested resources
		if (hex->region())
			hex->region()->sub(a_res);
	};

	/// Emits move section info.
	void PlantMod::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.plant_modify.res");
		section->line("dp.move.plant_modify.spread");

		// add arguments
		list["res"] = std::format("{}/{}/{}", a_res.money, a_res.berry, a_res.peach);
		list["pos"] = ext::str_vec(mid);
		list["radius"] = ext::str_int(radius);
		list["count"] = ext::str_int(a_target.size());
		list["skill_name"] = "@!dp.move.name.plant_modify";
	};
};
#include "game/logic/turn_logic.hpp"
#include "game/logic/skill_helper.hpp"

namespace logic {
	/// Executes turn transition logic.
	/// 
	/// @param map Map reference.
	History::UniqList turn(Map* map) {
		// map change list
		History::UniqList list;
		
		// visited regions indices
		std::set<size_t> visited;

		// update all regions
		for (int y = 0; y < map->size().y; y++) {
			for (int x = 0; x < map->size().x; x++) {
				// get tile
				Hex* hex = map->at({ x, y });

				// ignore if empty or not solid
				// or no attached region
				if (!hex || !hex->solid() || !hex->region()) continue;
				Region& reg = *hex->region();

				// ignore tile if already got processed
				// or tile does not have a region attached
				size_t idx = hex->region().index();
				if (visited.find(idx) != visited.cend())
					continue;

				// update region
				auto* move = new Moves::RegionChange({ x, y }, reg);
				reg.tick();
				move->state = reg;

				// mark region as processed
				visited.insert(idx);
			};
		};

		// entity state change
		auto change_state = [&](Entity* entity, bool grave) {
			// get troop tile
			HexRef tile = map->atref(entity->pos);

			// store initial state
			auto* move = new Moves::EntityChange(Moves::store_entity(tile.hex, tile.pos));

			// tick entity
			entity->tickState();
			if (entity->dead()) {
				if (grave) {
					Plant plant;
					plant.type = Plant::Grave;
					plant.pos = entity->pos;
					map->setPlant(plant);
				}
				else map->removeEntity(tile.hex);
			};

			// store new state
			move->state = Moves::store_entity(tile.hex, tile.pos);

			// store move
			list.push_back(std::unique_ptr<Move>(move));
		};

		// tick all troops
		auto troops = map->troopList();
		while (auto* troop = troops.next())
			change_state(troop, true);

		// tick all buildings
		auto builds = map->buildList();
		while (auto* build = builds.next())
			change_state(build, false);

		// tick all plants
		auto plants = map->plantList();
		while (auto* plant = plants.next())
			change_state(plant, false);

		// return entity changes
		return list;
	};
};
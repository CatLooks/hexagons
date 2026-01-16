#include "game/logic/turn_logic.hpp"
#include "game/logic/skill_helper.hpp"

namespace logic {
	/// Executes global turn transition logic.
	///
	/// @param map Map reference.
	History::UniqList global(Map* map) {
		// map change list
		History::UniqList list;

		// tick all plants
		auto plants = map->plantList();
		while (auto* plant = plants.next()) {
			// get plant tile
			HexRef tile = map->atref(plant->pos);

			// store initial state
			auto* move = new Moves::EntityChange(*plant);

			// spread the plant
			if (plant->spread_roll()) {
				// select solid tiles without any entities
				Spread spread = {
					.hop = skillf::solidHop,
					.pass = [=](const Spread::Tile& tile)
						{ return !tile.hex->entity(); }
				};

				// get tile list
				auto tlist = spread.applylist(*map, plant->pos, 1);
				if (!tlist.empty()) {
					// select random tile from list
					sf::Vector2i pos = tlist[rand() % tlist.size()];

					// plant creation move
					auto* move = new Moves::EntityChange(Moves::Empty { .pos = pos });
					{
						Plant nplant;
						nplant.type = logic::plant_spread[plant->type];
						nplant.pos = pos;

						map->setPlant(nplant);
						move->state = nplant;
					};
					list.push_back(std::unique_ptr<Move>(move));
				};
			};

			// tick plant state
			plant->tickState();

			// store new state
			move->state = *plant;
		};

		// return map changes
		return list;
	};

	/// Executes turn transition logic.
	History::UniqList turn(Map* map, Region::Team team) {
		// map change list
		History::UniqList list;

		// update all regions
		Regions::foreach(map, [&, team](Region& reg, sf::Vector2i pos) {
			if (reg.team != team) return;

			// record region state change
			auto* move = new Moves::RegionChange(pos, reg.data());
			reg.tick();
			move->state = reg.data();

			// register region change move
			list.push_back(std::unique_ptr<Move>(move));
		});

		// entity state change
		auto change_state = [&, team](Entity* entity, bool grave) {
			// get entity tile
			HexRef tile = map->atref(entity->pos);
			if (tile.hex->team != team) return;

			// store initial state
			auto* move = new Moves::EntityChange(Moves::store_entity(tile.hex, tile.pos));

			// tick entity
			entity->tickState();

			// check if entity is dead
			if (entity->dead() || (
				// kill troops in dead region
				grave && tile.hex->region() && tile.hex->region()->dead
			)) {
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

		// return map changes
		return list;
	};
};
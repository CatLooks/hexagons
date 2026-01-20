#include "game/logic/turn_logic.hpp"
#include "game/logic/skill_helper.hpp"

namespace logic {
	/// Executes turn transition logic.
	/// 
	/// @param map Map reference.
	History::UniqList turn(Map* map) {
		// map change list
		History::UniqList list;

		// update all regions
		Regions::foreach(map, [&list](Region& reg, sf::Vector2i pos) {
			// record region state change
			auto* move = new Moves::RegionChange(pos, reg.data());
			reg.tick();
			move->state = reg.data();

			// register region change move
			list.push_back(std::unique_ptr<Move>(move));
		});

		// entity state change
		auto change_state = [&](Entity* entity, bool grave) {
			// get troop tile
			HexRef tile = map->atref(entity->pos);

			// store initial state
			auto* move = new Moves::EntityChange(Moves::store_entity(tile.hex, tile.pos));

			// tick entity
			entity->tickState();

			// check if entity is dead
			if (entity->dead() || (
				// kill troops in dead region (ignore unclaimed)
				tile.hex->team != Region::Unclaimed
					&& tile.hex->region() && tile.hex->region()->dead
					&& dynamic_cast<Troop*>(entity)
			)) {
				if (grave) {
					Plant plant;
					plant.type = Plant::Grave;
					plant.pos = entity->pos;
					map->setPlant(plant);
				}
				else map->removeEntity(tile.hex);
			};

			// update plants
			if (auto* plant = dynamic_cast<Plant*>(entity)) {
				// grow plants
				if (rand() % 5 == 0) switch (plant->type) {
					case Plant::Sapling: plant->type = Plant::Tree; break;
					case Plant::Tree: plant->type = Plant::Peach; break;
					case Plant::Bush: plant->type = Plant::Berry; break;
				};

				// spread plants
				if ((plant->type == Plant::Bush || plant->type == Plant::Berry) && (rand() % 5 == 0)) {
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

						// bush creation move
						auto* move = new Moves::EntityChange(Moves::Empty { .pos = pos });
						{
							Plant plant;
							plant.type = Plant::Bush;
							plant.pos = pos;

							map->setPlant(plant);
							move->state = plant;
						};
						list.push_back(std::unique_ptr<Move>(move));
					};
				};
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
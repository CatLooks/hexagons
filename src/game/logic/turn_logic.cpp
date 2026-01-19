#include "game/logic/turn_logic.hpp"
#include "game/logic/skill_helper.hpp"
#include <algorithm>

namespace logic {
	/// Executes global turn transition logic.
	///
	/// @param map Map reference.
	History::UniqList global(Map* map) {
		// map change list
		History::UniqList list;

		// new plant list
		std::deque<Plant> nplants;

		// tick all plants
		auto plants = map->plantList();
		while (auto* plant = plants.next()) {
			// get plant tile
			HexRef tile = map->atref(plant->pos);

			// store initial state
			auto* move = new Moves::EntityChange(*plant);

			// spread the plant
			if (plant->spread_roll(map)) {
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

					// store new plant info
					Plant nplant;
					nplant.type = logic::plant_spread[plant->type];
					nplant.pos = pos;
					nplants.push_back(nplant);
				};
			};

			// tick plant state
			plant->tickState(map);

			// store new state
			move->state = *plant;
		};

		// construct new plants
		for (const auto& plant : nplants) {
			auto* move = new Moves::EntityChange(Moves::Empty{ .pos = plant.pos });
			{
				map->setPlant(plant);
				move->state = plant;
			};
			list.push_back(std::unique_ptr<Move>(move));
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
			entity->tickState(map);

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

	/// Counts tiles for every team.
	TileCount count(Map* map, const std::vector<Messages::Player>& players) {
		// create counter list
		TileCount count;
		for (int i = 0; i < Region::Count; i++)
			count.teams.push_back({ .team = static_cast<Region::Team>(i) });

		// count all solid tiles
		for (int y = 0; y < map->size().y; y++) {
			for (int x = 0; x < map->size().x; x++) {
				Hex* hex = map->at({ x, y });
				if (hex && hex->solid()) {
					// count total tiles
					count.total++;

					// count team tiles
					count.teams[hex->team].tiles++;
				};
			};
		};

		// find team in player list
		auto find = [&](Region::Team team) {
			return std::ranges::count_if(players, [=](const Messages::Player& player) {
				return player.team == team;
			}) != 0;
		};

		// sort list
		std::sort(count.teams.begin(), count.teams.end(),
			[&](const TileCount::TeamInfo& a, const TileCount::TeamInfo& b)
		{
			if (a.tiles == b.tiles) {
				// prioritize teams with assigned players
				return find(a.team) > find(b.team);
			};
			return a.tiles > b.tiles;
		});
		return count;
	};

	/// Returns a victor of the game.
	Region::Team win(const TileCount& count) {
		for (const auto& info : count.teams) {
			if ((float)info.tiles / count.total >= 0.7f)
				return info.team;
		};
		return Region::Unclaimed;
	};
};
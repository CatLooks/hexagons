#include "game/bot_ai.hpp"
#include "random.hpp"
#include "ui/units.hpp"

#include "game/values/troop_values.hpp"
#include "game/values/build_values.hpp"
#include "game/values/plant_values.hpp"
#include <map>

namespace ai {
	/// Returns available moves for a skill.
	Spread::List select(
		Map& map, const Skill& skill,
		SkillState& state,
		Hex* hex, sf::Vector2i pos
	) {
		// get selection index
		size_t idx = map.newSelectionIndex();
		
		// get move list
		auto spread = skill.select(state, { hex, pos }, idx);
		auto list = spread.applylist(map, pos, skill.radius);

		// stop selection
		map.stopSelection();
		return list;
	};

	/// Randomly returns a position with the highest ranking.
	std::optional<sf::Vector2i> get(
		const Spread::List& list,
		std::function<void(sf::Vector2i, int&)> rank,
		float diff
	) {
		using Pair = std::pair<sf::Vector2i, int>;

		// ignore if the list is empty
		if (list.empty()) return {};

		// randomly ignore movement
		if (Random::chance(ui::lerpf(0.3f, 0.04f, diff)))
			return {};

		// compute ranks for each position
		std::vector<Pair> arr;
		for (sf::Vector2i pos : list) {
			Pair p = { pos, 0 };
			rank(pos, p.second);
			arr.push_back(p);
		};

		// sort rank list
		std::sort(arr.begin(), arr.end(), [=](const Pair& a, const Pair& b) {
			return a.second > b.second;
		});

		// ignore if highest rank is negative
		if (arr[0].second < 0) return {};

		// choose a random position with high rank
		size_t limit = 1;
		while (limit < arr.size()) {
			if (arr[limit].second < arr[0].second)
				break;
			limit++;
		};

		// increase limit according to difficulty
		limit = (size_t)(limit * ui::lerpf(1.0f, 1.5f, Random::uniform() * diff));
		limit = std::min(limit, arr.size());

		// return random position
		return arr[Random::u16() % limit].first;
	};

	/// Returns a random empty spot around a tile.
	/// 
	/// @param map Map reference.
	/// @param pos Tile position.
	/// @param radius Search radius.
	/// 
	/// @return Random position.
	std::optional<sf::Vector2i> empty(
		Map& map,
		sf::Vector2i pos,
		size_t radius,
		Region::Team team
	) {
		// get empty tiles around
		static const Spread spread = {
			.hop = skillf::solidHop,
			.pass = [=](const Spread::Tile& tile)
				{ return tile.hex->team == team && !tile.hex->entity(); }
		};
		auto list = spread.applylist(map, pos, radius);

		// select a random one
		if (list.empty()) return {};
		return list[Random::u16() % list.size()];
	};

	/// Generates a list of moves for a team.
	History::UniqList generate(Map& map, Region::Team team, float diff) {
		// generate move list for each region
		Regions::foreach(&map, [=, &map](Region& reg, sf::Vector2i pos) {
			// ignore if not target region
			if (reg.team != team) return;

			// total region power
			int power = 0;

			// search for entities in region
			Spread spread = {
				// spread in the same region
				.hop = [=](const Spread::Tile& tile)
					{ return tile.hex->solid() && tile.hex->team == team; },

				// pass if an entity is present
				.pass = [=](const Spread::Tile& tile)
					{ return tile.hex->entity(); },

				// add tile power
				.effect = [&power](const Spread::Tile& tile) {
					if (tile.hex->troop)
						power += tile.hex->troop->type;
				},

				// don't ignore point of entry
				.imm = true
			};
			auto list = spread.applylist(map, pos);

			// dummy skill state
			SkillState skill_state = { .map = &map, .region = &reg };

			// buy a new troop
			if (Random::chance(reg.income * 10.f / power * diff)) {
				// find an empty spot
				Spread spr = {
					.hop = [=](const Spread::Tile& tile)
						{ return tile.hex->solid() && tile.hex->team == team; },
					.pass = [=](const Spread::Tile& tile)
						{ return !tile.hex->entity(); },
					.imm = true
				};
				auto emp = spr.applylist(map, pos);
				if (!emp.empty()) {
					// pick a unit for reinforcement
					int unit = Troop::Knight;
					while (
						(logic::troop_cost[unit] * 2 > reg.money
							|| logic::troop_upkeep[unit] * 2 > reg.income)
						&& unit >= 0
						) unit--;

					// place reinforcement unit
					if (unit >= 0) {
						sf::Vector2i pos = emp[Random::u16() % emp.size()];
						//printf("[AI] Buying reinforcement %s at %d, %d\n", Values::troop_names[unit], pos.x, pos.y);

						Troop troop;
						troop.type = static_cast<Troop::Type>(unit);
						troop.hp = troop.max_hp();
						troop.pos = pos;
						auto* move = new Moves::EntityPlace(troop, reg);
						map.executeSkill(move, troop.pos, &SkillList::buy_troop);
					};
				};
			};

			// for each entity
			for (sf::Vector2i pos : list) {
				Hex* hex = map.at(pos);
				HexRef ref = { hex, pos };

				// branch on entity type
				if (hex->troop) {
					Troop& troop = *hex->troop;

					// ignore troop with a chance
					if (Random::chance(ui::lerpf(0.2f, 0.f, diff)))
						continue;

					// get move positions
					auto list = select(map, SkillList::move, skill_state, hex, pos);

					// rank move positions
					auto target = get(list, [=, &map](sf::Vector2i pos, int& rank) {
						Hex* now = map.at(pos);

						// add danger level
						skillf::checkAround(&map, pos, 2, [=, &rank, &troop](const Spread::Tile& tile) {
							if (tile.hex->team != hex->team && tile.hex->troop)
								rank += (hex->troop->offense(Access::Query).pts
									- tile.hex->troop->offense(Access::Query).pts) * 5;
							return false;
						}, Spread::Def);
						
						// add plant bonus
						if (now->plant) {
							static const int table[Plant::Count]
								= { 0, -2, 0, -1, -3, 1, 1 };
							rank += table[now->plant->type]
								* (now->team == hex->team ? 1 : -1);
						};

						// repaint enemy team
						if (now->team != hex->team) {
							rank += now->team == Region::Unclaimed ? 2 : 1;

							// reward region filling
							rank += (int)skillf::checkAround(
								&map, pos, 1,
								[=](const Spread::Tile& tile)
									{ return tile.hex->team == hex->team; },
								Spread::Def
							) - 1;

							// add troop kill bonus
							if (now->troop) rank += now->troop->type * 2;

							// add building destroy bonus
							if (now->build) rank += 2;
						};
					}, diff);

					// execute move
					if (target) {
						auto* move = new Moves::TroopMove(*target);
						map.executeSkill(move, troop.pos, &SkillList::move);

						// resample hex
						hex = map.at(*target);
						pos = *target;
					};

					// check if attack is off cooldown
					if (troop.timers[1] == 0) {
						if (troop.type != Troop::Farmer) {
							// get target list
							auto skill = logic::troop_skills[troop.type].skills[1];
							auto list = select(map, *skill, skill_state, hex, pos);

							// rank targets
							auto target = get(list, [=, &map](sf::Vector2i pos, int& rank) {
								Hex* hex = map.at(pos);
								Entity* ent = hex->entity();

								// prioritize low hp troops
								rank += ent->max_hp() - ent->hp;
							}, diff);

							// execute attack
							if (target) {
								auto* move = new Moves::TroopAttack(*target);
								map.executeSkill(move, troop.pos, skill);
							};
						};
					};

					// check if need reinforcements
					int rank = 0;
					Spread spread = {
						.hop = skillf::solidHop,
						.pass = [=](const Spread::Tile& tile)
							{ return (bool)tile.hex->troop; },
						.effect = [=, &rank](const Spread::Tile& tile) {
							if (tile.hex->team == hex->team)
								rank -= tile.hex->troop->type / 2;
							else
								rank += tile.hex->troop->type;
						}
					};
					spread.apply(map, pos, 2);
					if (Random::chance(rank / 12.f)) {
						// choose reinforcement type
						if ((rank < 4) ^ Random::chance(ui::lerpf(0.5f, 0.05f, diff))) {
							// get random empty spot nearby
							auto target = empty(map, pos, 1, reg.team);
							if (target) {
								// place a tower
								Build build;
								build.type = Build::Tower;
								build.hp = build.max_hp();
								build.pos = *target;
								auto* move = new Moves::EntityPlace(build, reg);
								map.executeSkill(move, build.pos, &SkillList::buy_build);
							};
						}
						else {
							// get random empty spot nearby
							auto target = empty(map, pos, 3, reg.team);
							if (target) {
								// pick a unit for reinforcement
								int unit = (int)(rank / 2.f + ui::lerpf(-0.75f, 0.75f, Random::uniform()));
								while (
									(logic::troop_cost[unit] * 2 > reg.money
										|| logic::troop_upkeep[unit] * 2 > reg.income)
									&& unit > 0
									) unit--;

								// place reinforcement unit
								Troop troop;
								troop.type = static_cast<Troop::Type>(unit);
								troop.hp = troop.max_hp();
								troop.pos = *target;
								auto* move = new Moves::EntityPlace(troop, reg);
								map.executeSkill(move, troop.pos, &SkillList::buy_troop);
							};
						};
					};
				}
				else if (hex->build) {
					Build& build = *hex->build;

					// farm logic
					if (build.type == Build::Farm) {
						float new_farm = reg.money / (4.f * logic::build_cost(Build::Farm, reg));

						// new farm chance
						if (Random::chance(new_farm * ui::lerpf(0.5f, 1.1f, diff))) {
							// get random empty spot around
							auto target = empty(map, pos, Random::chance(0.15f) ? 2 : 1, reg.team);
							if (target) {
								// check if hex is in region
								Build farm;
								farm.type = Build::Farm;
								farm.pos = *target;
								auto* move = new Moves::EntityPlace(farm, reg);
								map.executeSkill(move, build.pos, &SkillList::buy_build);
							};
						};
					};

					// beacon logic
					if (build.type == Build::Beacon) {
						// get enemy troop ranking
						auto list = select(map, SkillList::stun, skill_state, hex, pos);
						int rank = 0;

						for (sf::Vector2i pos : list) {
							Hex* now = map.at(pos);
							if (now->team != hex->team && now->troop)
								rank += now->troop->type / 2;
						};

						// stun enemies if enough troops
						if (Random::chance(rank / 15.f)) {
							// fail some opportunities
							if (!Random::chance(ui::lerpf(0.5f, 0.2f, diff))) {
								auto* move = SkillList::stun.action(skill_state, map, ref, ref);
								map.executeSkill(move, build.pos, &SkillList::stun);
							};
						};
					};
				}
				else if (hex->plant) {
					Plant& plant = *hex->plant;

					// get plant group size
					size_t count = skillf::checkAround(
						&map, pos, 1,
						[=](const Spread::Tile& tile)
							{ return tile.hex->plant && tile.hex->plant->type != Plant::Grave; },
						Spread::Def
					);
					if (Random::chance(count / 12.f)) {
						// get random free spot near
						Spread spread = {
							.hop = skillf::solidHop,
							.pass = skillf::emptyPass
						};
						auto list = spread.applylist(map, pos, 1);
						if (!list.empty()) {

						};
					};
				};
			};


		});

		// return empty list lol
		return {};
	};

	/// Returns a list of all map teams.
	std::vector<Region::Team> teams(Map& map, Region::Team first) {
		std::vector<Region::Team> list;
		Regions::foreach(&map, [first, &list](Region& reg, sf::Vector2i pos) {
			// ignore unclaimed regions
			if (reg.team == Region::Unclaimed) return;

			// check if team has not been added
			if (std::find(list.begin(), list.end(), reg.team) == list.cend()) {
				// add team to list
				list.push_back(reg.team);

				// swap with first team if prioritized
				if (reg.team == first)
					std::swap(list[0], list[list.size() - 1]);
			};
		});
		return list;
	};
};
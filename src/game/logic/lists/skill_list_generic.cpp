#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// ======== ENTITY WITHDRAW ======== ///
	const Skill withdraw = {
		.type = Skills::Withdraw,
		.annotation = Skill::None,
		.action = [](const SkillState&, Map& map, const HexRef& tile, const HexRef& _) {
			// create withdraw move
			return new Moves::EntityWithdraw(tile.pos);
		},
		.format = Skill::Self
	};

	/// ======== TROOP MOVE ======== ///
	const Skill move = {
		.type = Skills::Move,
		.annotation = Skill::Aim,
		.select = [](const SkillState& state, const HexRef& tile, size_t idx) {
			return Spread {
				.hop = [=](const Spread::Tile& now) {
					// ignore if not solid tile
					if (!now.hex->solid())
						return false;

					// check if not near own region
					if (skillf::checkAround(state.map, now.pos, 1, [=](const Spread::Tile& nb) {
						return nb.hex->region() == tile.hex->region();
					}, Spread::Alt) == 0) return false;

					// get entites at tiles
					Troop& sub = *tile.hex->troop;
					Entity* ent = now.hex->entity();

					// override checks
					// ignore if entity is a plant
					if (ent && !now.hex->plant) {
						// enemy override checks
						if (now.hex->region() != tile.hex->region()) {
							// ignore if moving onto a troop
							if (dynamic_cast<Troop*>(ent)) {
								// and attack is on cooldown
								int id = sub.skill_id(sub.skill_into(ent));
								if (id != -1 && sub.timers[id] > 0)
									return false;
							};

							// get attacking troop damage
							auto dmg = sub.offense(Access::Query);

							// check if enough damage for an instakill
							return ent->instakill(dmg);
						}

						// friendly override checks
						else {
							// check if troop merge is possible
							if (now.hex->troop) {
								Troop::Type merge = logic::troop_merge[sub.type][now.hex->troop->type];
								return merge < Troop::Count;
							};

							// disable override of friendly entities
							return false;
						};
					};

					// hop successful
					return true;
				},
				.pass = [=](const Spread::Tile& now) {
					// check if tile is not guarded by a building
					return skillf::checkAround(
						state.map, now.pos, (size_t)logic::defense_range,
						[=](const Spread::Tile& nb)
					{
						// ignore if same region or no building
						if (nb.hex->region() == tile.hex->region() || !nb.hex->build)
							return false;

						// get building guard potential
						Troop::Type guard = logic::build_guard[nb.hex->build->type];

						// disable move if potential is greater than the troop
						return tile.hex->troop->type < guard;
					}, Spread::Alt) == 0;
				},
				.effect = skillf::selectTile(idx)
			};
		},
		.radius = 2,
		.action = [](const SkillState&, Map& map, const HexRef& prev, const HexRef& next) -> Move* {
			// ignore if no troop
			if (!prev.hex->troop) return nullptr;

			// create merge move
			if (next.hex->troop && next.hex->region() == prev.hex->region())
				return new Moves::TroopMerge(next.pos);

			// otherwise create movement move
			return new Moves::TroopMove(next.pos);
		},
		.format = Skill::SingleAim,
		.reselect = true
	};
};
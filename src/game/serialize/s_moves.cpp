#include "game/serialize/moves.hpp"

/// Check if an instance of `move` is of subtype `T`, then store pointer into `data`.
#define IF(T) if (auto* data = dynamic_cast<const T*>(move))

namespace Serialize {
	/// Encodes a move into a packet.
	void encodeMove(sf::Packet& packet, const Move* move) {
		// store generic info
		packet << move->skill_cooldown;
		if (move->skill_cooldown != 0) {
			packet << move->skill_pos;
			packet << (uint8_t)move->skill_type;
		};

		// entity place
		IF(Moves::EntityPlace) {
			packet << (uint8_t)M_EntityPlace;
			packet << data->entity;
			packet << data->var;
		};
		// entity withdraw
		IF(Moves::EntityWithdraw) {
			packet << (uint8_t)M_EntityWithdraw;
			packet << data->pos;
		};
		// entity effect
		IF(Moves::EntityEffect) {
			packet << (uint8_t)M_EntityEffect;
			packet << data->pos;
			packet << static_cast<uint8_t>(data->effect);
			packet << data->peach;
		};

		// troop move
		IF(Moves::TroopMove) {
			packet << (uint8_t)M_TroopMove;
			packet << data->dest;
		};
		// troop attack
		IF(Moves::TroopAttack) {
			packet << (uint8_t)M_TroopAttack;
			packet << data->dest;
		};
		// troop heal
		IF(Moves::TroopHeal) {
			packet << (uint8_t)M_TroopHeal;
			packet << data->mid;
			packet << data->radius;
			packet << data->heal;
			packet << (uint8_t)data->team;
		};

		// plant cut
		IF(Moves::PlantCut) {
			packet << (uint8_t)M_PlantCut;
			packet << data->pos;
		};
		// plant modify
		IF(Moves::PlantMod) {
			packet << (uint8_t)M_PlantMod;
			packet << data->mid;
			packet << data->radius;
		};

		// radius effect
		IF(Moves::RadiusEffect) {
			packet << (uint8_t)M_RadiusEffect;
			packet << data->mid;
			packet << data->radius;
			packet << static_cast<uint8_t>(data->effect);
			packet << (uint8_t)data->team;
		};
		// entity change
		IF(Moves::EntityChange) {
			packet << (uint8_t)M_EntityChange;
			packet << data->state;
		};
		// region change
		IF(Moves::RegionChange) {
			packet << (uint8_t)M_RegionChange;
			packet << data->state.res();
			packet << data->state.var();
			packet << data->state.dead;
		};
	};

	/// Decodes a move from a packet.
	std::unique_ptr<Move> decodeMove(sf::Packet& packet) {
		// get generic info
		auto cd = from<uint8_t>(packet);
		sf::Vector2i pos = cd ? from<sf::Vector2i>(packet) : sf::Vector2i();
		uint8_t tex = cd ? from<uint8_t>(packet) : 0;

		// ignore if bad skill type
		if (tex >= Skills::Count) return {};

		// get move type
		auto type = from<uint8_t>(packet);
		if (type >= M_Count) return {};

		// decode move info
		Move* res = nullptr;
		switch (type) {
			// entity moves
			case M_EntityPlace: res = new Moves::EntityPlace(
				from<Moves::EntState>(packet),
				from<RegionVar>(packet)
			); break;
			case M_EntityWithdraw: res = new Moves::EntityWithdraw(
				from<sf::Vector2i>(packet)
			); break;
			case M_EntityEffect: res = new Moves::EntityEffect(
				from<sf::Vector2i>(packet),
				static_cast<EffectType>(from<uint8_t>(packet)),
				from<int>(packet)
			); break;

			// troop moves
			case M_TroopMove: res = new Moves::TroopMove(
				from<sf::Vector2i>(packet)
			); break;
			case M_TroopAttack: res = new Moves::TroopAttack(
				from<sf::Vector2i>(packet)
			); break;
			case M_TroopHeal: res = new Moves::TroopHeal(
				from<sf::Vector2i>(packet),
				from<size_t>(packet),
				from<int>(packet),
				static_cast<Region::Team>(from<uint8_t>(packet))
			); break;

			// building moves
			case M_RadiusEffect: res = new Moves::RadiusEffect(
				from<sf::Vector2i>(packet),
				from<size_t>(packet),
				static_cast<EffectType>(from<uint8_t>(packet)),
				static_cast<Region::Team>(from<uint8_t>(packet))
			); break;

			// plant moves
			case M_PlantCut: res = new Moves::PlantCut(
				from<sf::Vector2i>(packet)
			); break;
			case M_PlantMod: res = new Moves::PlantMod(
				from<sf::Vector2i>(packet),
				from<size_t>(packet)
			); break;
			
			// state change moves
			case M_EntityChange: {
				auto* move = new Moves::EntityChange({});
				move->state = from<Moves::EntState>(packet);
				res = move;
			}; break;
			case M_RegionChange: {
				auto* move = new Moves::RegionChange(pos, {});
				move->state = {
					from<RegionRes>(packet),
					from<RegionVar>(packet),
					from<bool>(packet)
				};
				res = move;
			}; break;
		};

		// set generic info
		if (res) {
			res->skill_pos = pos;
			res->skill_type = static_cast<Skills::Type>(tex);
			res->skill_cooldown = cd;
		};
		return std::unique_ptr<Move>(res);
	};
};
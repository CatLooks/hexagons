#include "game/serialize/moves.hpp"
#include <iostream>
#include <typeinfo>
#include <cassert>

/// Check if an instance of `move` is of subtype `T`, then store pointer into `data`.
#define IF(T) if (auto* data = dynamic_cast<const T*>(move))

namespace Serialize {

	/// Encodes a move into a packet.
	void encodeMove(sf::Packet& packet, const Move* move) {
		// Encodes cooldown header (Safe)
		uint8_t cd = static_cast<uint8_t>(move->skill_cooldown);
		packet << cd;

		if (cd != 0) {
			packet << move->skill_pos;
			packet << static_cast<uint8_t>(move->skill_type);
		};

		// -----------------------------------------------------------
		// ENCODING IS LEFT-TO-RIGHT.
		// DECODING MUST BE STRICTLY LEFT-TO-RIGHT TOO.
		// -----------------------------------------------------------

		// --- Entity Moves ---
		IF(Moves::EntityPlace) {
			packet << (uint8_t)M_EntityPlace;
			packet << data->entity; 
			packet << data->var;    
		}
		else IF(Moves::EntityWithdraw) {
			packet << (uint8_t)M_EntityWithdraw;
			packet << data->pos;
		}
		else IF(Moves::EntityEffect) {
			packet << (uint8_t)M_EntityEffect;
			packet << data->pos;
			packet << static_cast<uint8_t>(data->effect);
			packet << static_cast<int32_t>(data->peach);
		}

		// --- Troop Moves ---
		else IF(Moves::TroopAttack) {
			packet << (uint8_t)M_TroopAttack;
			packet << data->dest;
		}
		else IF(Moves::TroopMove) {
			packet << (uint8_t)M_TroopMove;
			packet << data->dest;
		}
		else IF(Moves::TroopHeal) {
			packet << (uint8_t)M_TroopHeal;
			packet << static_cast<sf::Vector2i>(data->mid);
			packet << static_cast<uint64_t>(data->radius);
			packet << static_cast<int32_t>(data->heal);
			packet << static_cast<uint8_t>(data->team);
		}

		// --- Plant Moves ---
		else IF(Moves::PlantCut) {
			packet << (uint8_t)M_PlantCut;
			packet << data->pos;
		}
		else IF(Moves::PlantMod) {
			packet << (uint8_t)M_PlantMod;
			packet << static_cast<sf::Vector2i>(data->mid);
			packet << static_cast<uint64_t>(data->radius);
		}

		// --- Building / Global Moves ---
		else IF(Moves::RadiusEffect) {
			packet << (uint8_t)M_RadiusEffect;
			packet << static_cast<sf::Vector2i>(data->mid);
			packet << static_cast<uint64_t>(data->radius);
			packet << static_cast<uint8_t>(data->effect);
			packet << static_cast<uint8_t>(data->team);
		}

		// --- State Changes ---
		else IF(Moves::EntityChange) {
			packet << (uint8_t)M_EntityChange;
			packet << data->state;
		}
		else IF(Moves::RegionChange) {
			packet << (uint8_t)M_RegionChange;
			packet << data->state.res();
			packet << data->state.var();
			packet << data->state.dead;
		}
		else {
			std::cerr << "CRITICAL: Unknown move type in encodeMove: " 
					  << typeid(*move).name() << std::endl;
			assert(false && "Unknown move type serialization");
		}
	};

	/// Decodes a move from a packet.
	std::unique_ptr<Move> decodeMove(sf::Packet& packet) {
		auto cd = from<uint8_t>(packet);
		sf::Vector2i pos = cd ? from<sf::Vector2i>(packet) : sf::Vector2i();
		uint8_t tex = cd ? from<uint8_t>(packet) : 0;

		if (tex >= Skills::Count) return {};

		auto type = from<uint8_t>(packet);
		if (type >= M_Count) return {};

		Move* res = nullptr;
		
		// -------------------------------------------------------------
		// FIX: Force read order by using temporary variables.
		// Function arguments `new T(from(p), from(p))` order is UNDEFINED.
		// -------------------------------------------------------------
		
		switch (type) {
			case M_EntityPlace: {
				auto ent = from<Moves::EntState>(packet); // Read 1st
				auto var = from<RegionVar>(packet);       // Read 2nd
				res = new Moves::EntityPlace(std::move(ent), var);
			} break;

			case M_EntityWithdraw: {
				// Only 1 arg, safe.
				res = new Moves::EntityWithdraw(from<sf::Vector2i>(packet));
			} break;

			case M_EntityEffect: {
				auto p = from<sf::Vector2i>(packet);
				auto e = static_cast<EffectType>(from<uint8_t>(packet));
				auto v = from<int>(packet);
				res = new Moves::EntityEffect(p, e, v);
			} break;

			case M_TroopMove: {
				res = new Moves::TroopMove(from<sf::Vector2i>(packet));
			} break;

			case M_TroopAttack: {
				res = new Moves::TroopAttack(from<sf::Vector2i>(packet));
			} break;

			case M_TroopHeal: {
				auto mid  = from<sf::Vector2i>(packet);
				auto rad  = from<size_t>(packet); // Ensure size_t matches uint64_t from writer
				auto heal = from<int>(packet);
				auto team = static_cast<Region::Team>(from<uint8_t>(packet));
				res = new Moves::TroopHeal(mid, rad, heal, team);
			} break;

			case M_RadiusEffect: {
				auto mid  = from<sf::Vector2i>(packet);
				auto rad  = from<size_t>(packet);
				auto eff  = static_cast<EffectType>(from<uint8_t>(packet));
				auto team = static_cast<Region::Team>(from<uint8_t>(packet));
				res = new Moves::RadiusEffect(mid, rad, eff, team);
			} break;

			case M_PlantCut: {
				res = new Moves::PlantCut(from<sf::Vector2i>(packet));
			} break;

			case M_PlantMod: {
				auto mid = from<sf::Vector2i>(packet);
				auto rad = from<size_t>(packet);
				res = new Moves::PlantMod(mid, rad);
			} break;
			
			case M_EntityChange: {
				auto* move = new Moves::EntityChange({});
				move->state = from<Moves::EntState>(packet);
				res = move;
			}; break;

			case M_RegionChange: {
				// Initializer lists {} guarantee left-to-right evaluation order
				// so this block was actually okay, but let's keep it consistent.
				auto r = from<RegionRes>(packet);
				auto v = from<RegionVar>(packet);
				auto d = from<bool>(packet);

				auto* move = new Moves::RegionChange(pos, {});
				move->state = { r, v, d };
				res = move;
			}; break;
		};

		if (res) {
			res->skill_pos = pos;
			res->skill_type = static_cast<Skills::Type>(tex);
			res->skill_cooldown = cd;
		};
		return std::unique_ptr<Move>(res);
	};
};
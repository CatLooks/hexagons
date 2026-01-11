#pragma once

// include dependencies
#include "general.hpp"
#include "entities.hpp"
#include "game/moves.hpp"

namespace Serialize {
	/// Move enumeration.
	enum MoveType {
		// entity moves
		M_EntityPlace,
		M_EntityWithdraw,
		M_EntityEffect,

		// troop moves
		M_TroopMove,
		M_TroopAttack,
		M_TroopHeal,

		// building moves
		M_RadiusEffect,

		// plant moves
		M_PlantCut,
		M_PlantMod,

		// state change moves
		M_EntityChange,
		M_RegionChange,
		M_Count
	};

	/// Encodes a move into a packet.
	/// 
	/// @param packet Target packet.
	/// @param move Serialized move.
	void encodeMove(sf::Packet& packet, const Move* move);

	/// Decodes a move from a packet.
	/// 
	/// @param packet Target packet.
	/// 
	/// @return Deserialized move.
	std::unique_ptr<Move> decodeMove(sf::Packet& packet);
};
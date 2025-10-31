#include "game/hex.hpp"

/// Checks if a tile is ground-like.
bool Hex::isSolid() const {
	return type == Ground || type == Bridge;
};

namespace Name {
	/// Returns a request value for team name.
	const char* team(Hex::Team id) {
		static const char* table[Hex::Count] = {
			"@!team.v", "@!team.r", "@!team.o",
			"@!team.y", "@!team.g", "@!team.a",
			"@!team.b", "@!team.p", "@!team.m",
		};
		return (id >= 0 && id < Hex::Count)
			? table[id] : "@!team.bad";
	};
	/// Returns a request value for team name.
	const char* team(const Hex& entity) {
		return team(entity.team);
	};
};
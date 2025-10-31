#include "game/troop.hpp"

namespace Name {
	/// Returns the name of a troop.
	const char* troop(Troop::Type id) {
		static const char* table[Troop::Count] = {
			"@!troop.farmer",
			"@!troop.lumber",
			"@!troop.spear",
			"@!troop.archer",
			"@!troop.baron",
			"@!troop.knight"
		};
		return (id >= 0 && id < Troop::Count)
			? table[id] : "@!troop.bad";
	};
	/// Returns the name of a troop.
	const char* troop(const Troop& entity) {
		return troop(entity.type);
	};
};
#include "game/logic/troop_logic.hpp"

namespace logic {
	/// Troop max health.
	const int troop_hp[Troop::Count] = { 1, 2, 3, 2, 4, 6 };

	/// Base troop damage.
	const int troop_dmg_base[Troop::Count] = { 0, 1, 2, 1, 3, 4 };

	/// Troop buying cost.
	const int troop_cost[Troop::Count] = { 10, 15, 20, 25, 30, 40 };

	/// Troop upkeep cost.
	const int troop_upkeep[Troop::Count] = { 2, 4, 6, 14, 18, 36 };

	/// Returns troop damage.
	int troop_dmg(const Troop& troop) {
		return troop_dmg_base[troop.type];
	};

	/// Returns troop defense.
	int troop_def(const Troop& troop) {
		return troop.hp;
	};

	/// Returns troop range.
	int troop_rng(const Troop& troop) {
		return troop.type == Troop::Archer ? 3 : 1;
	};
};
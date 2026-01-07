#include "game/moves/move.hpp"
#include "game/map.hpp"

#include "game/values/entity_values.hpp"
#include "game/values/troop_values.hpp"
#include "game/values/build_values.hpp"
#include "game/values/plant_values.hpp"

#include "assets.hpp"

/// Returns tile to select after applying the move.
std::optional<sf::Vector2i> Move::applyCursor() {
	return {};
};

/// Returns tile to select after reverting the move.
std::optional<sf::Vector2i> Move::revertCursor() {
	return {};
};

/// Applies the move.
void Move::apply(Map* map) {
	// add cooldown
	if (skill_cooldown)
		if (Hex* hex = map->at(skill_pos))
			if (Entity* ent = hex->entity())
				ent->add_cooldown(skill_type, skill_cooldown);

	// apply the move
	onApply(map);
};

/// Reverts the move.
void Move::revert(Map* map) {
	// apply the move
	onRevert(map);

	// remove cooldown
	if (skill_cooldown)
		if (Hex* hex = map->at(skill_pos))
			if (Entity* ent = hex->entity())
				ent->sub_cooldown(skill_type, skill_cooldown);
};

namespace Moves {
	/// Returns entity state of a hex.
	EntState store_entity(const Hex* hex, sf::Vector2i pos) {
		if (hex) {
			if (hex->troop) return *hex->troop;
			if (hex->build) return *hex->build;
			if (hex->plant) return *hex->plant;
		};
		return Empty{ .pos = pos };
	};

	/// Places an entity on a map.
	void place_entity(const EntState* entity, Map* map) {
		// place empty space
		if (auto* data = std::get_if<Empty>(entity))
			map->removeEntity(map->at(data->pos));

		// place entities
		if (auto* data = std::get_if<Troop>(entity)) map->setTroop(*data);
		if (auto* data = std::get_if<Build>(entity)) map->setBuild(*data);
		if (auto* data = std::get_if<Plant>(entity)) map->setPlant(*data);
	};

	/// Returns entity state's position.
	sf::Vector2i entity_pos(const EntState* entity) {
		if (auto* data = std::get_if<Empty>(entity)) return data->pos;
		if (auto* data = std::get_if<Troop>(entity)) return data->pos;
		if (auto* data = std::get_if<Build>(entity)) return data->pos;
		if (auto* data = std::get_if<Plant>(entity)) return data->pos;
		return {}; // fallback
	};

	/// Returns string representation of an entity.
	std::string str_ent(const EntState* entity) {
		// no entity
		if (auto* data = std::get_if<Empty>(entity))
			return "@!dp.entity_state.none";
		const Entity* ent = nullptr;

		// get entity name
		std::string name = "@!dp.entity_state.unknown";
		if (auto* data = std::get_if<Troop>(entity)) {
			name = Values::troop_names[data->type];
			ent = &*data;
		};
		if (auto* data = std::get_if<Build>(entity)) {
			name = Values::build_names[data->type];
			ent = &*data;
		};
		if (auto* data = std::get_if<Plant>(entity)) {
			name = Values::plant_names[data->type];
			ent = &*data;
		};

		// return entity status
		return assets::lang::locale.req("dp.entity_state.format").get({
			{ "name", name },
			{ "hp", std::format("{}", ent ? ent->hp : 0) },
			{ "max", std::format("{}", ent ? ent->max_hp() : 0) }
		}, &assets::lang::locale);
	};

	/// Returns string representation of an effect list.
	std::string str_eff(const Entity::Effects& list) {
		// return special string if empty
		if (list.empty()) return "@!dp.empty";

		// effect list
		std::string effects;
		for (EffectType effect : list) {
			// separators
			if (!effects.empty())
				effects.append(", ");
			else
				effects.append("[ ");

			// short effect name
			effects.append(
				assets::lang::locale.req(Values::effect_names_short[static_cast<int>(effect)])
				.get({}, &assets::lang::locale)
			);
		};

		// ending
		effects.append(" ]");
		return effects;
	};
};
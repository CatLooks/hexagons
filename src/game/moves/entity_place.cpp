#include "game/moves/entity_place.hpp"
#include "game/map.hpp"

#include "game/logic/troop_logic.hpp"
#include "game/logic/build_logic.hpp"
#include "game/logic/plant_logic.hpp"

namespace Moves {
	/// Returns placement cost of an entity.
	int entity_cost(const Entity* entity, const RegionVar& var) {
		if (auto* data = dynamic_cast<const Troop*>(entity))
			return logic::troop_cost[data->type];
		if (auto* data = dynamic_cast<const Build*>(entity))
			return logic::build_cost(data->type, var);
		return 0;
	};

	/// Constructs an entity placement move.
	EntityPlace::EntityPlace(EntState entity, const RegionVar& var)
		: entity(entity), var(var) {};

	/// Applies the move.
	void EntityPlace::onApply(Map* map) {
		// get entity position
		sf::Vector2i pos = entity_pos(&entity);

		// get target tile
		Hex* hex = map->at(pos);
		if (!hex) return;

		// place entity
		if (auto* data = std::get_if<Troop>(&entity))
			map->setTroop(*data);
		if (auto* data = std::get_if<Build>(&entity))
			map->setBuild(*data);

		// subtract entity cost
		if (hex->region())
			hex->region()->money -= entity_cost(hex->entity(), var);
	};

	/// Reverts the move.
	void EntityPlace::onRevert(Map* map) {
		// get entity position
		sf::Vector2i pos = entity_pos(&entity);

		// get target tile
		Hex* hex = map->at(pos);
		if (!hex) return;

		// recompensate entity cost
		if (hex->region())
			hex->region()->money += entity_cost(hex->entity(), var);

		// remove entity
		map->removeEntity(hex);
	};

	/// Emits move section info.
	void EntityPlace::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.entity_place.what");

		// add arguments
		list["pos"] = ext::str_vec(entity_pos(&entity));
		list["entity"] = str_ent(&entity);
		list["skill_name"] = "@!dp.move.name.entity_place";
	};
};
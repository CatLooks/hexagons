#include "game/draw/entity_draw.hpp"
#include "game/values/entity_values.hpp"
#include "ui/units.hpp"

namespace Draw {
	/// Constructs a health bar object.
	/// 
	/// @param entity Target entity.
	Bar::Bar(const Entity& entity):
		hp(entity.hp),
		max(entity.max_hp()),
		poison(entity.hasEffect(EffectType::Poisoned)),
		effects(entity.effectList())
	{};

	/// Checks whether the health bar is fully filled.
	bool Bar::filled() const {
		return hp == max;
	};

	/// Draws health bar for an entity on the map.
	void Bar::drawAbove(sf::Vector2i pos, ui::RenderBuffer& buffer) const {
		// get health bar texture
		sf::IntRect texture = Values::hp_bar(hp, max, poison);

		// draw health bar panel
		buffer.quad({
			{
				pos.x + (Values::tileSize.x - texture.size.x * Values::k) / 2,
				pos.y + Values::tileSize.y / 32
			},
			texture.size * Values::k
		}, texture);
		buffer.forward(&assets::tilemap);

		// get position of the first icon
		sf::Vector2i icon_pos = {
			pos.x + Values::tileSize.x / 2
				- (int)effects.size() * Values::effect_textures[0].size.x * Values::k / 2,
			pos.y - Values::tileSize.y / 4
		};

		// draw effect icons
		for (EffectType effect : effects) {
			// draw icon
			sf::IntRect icon = Values::effect_textures[static_cast<int>(effect)];
			buffer.quad({ icon_pos, icon.size * Values::k }, icon);

			// increment x
			icon_pos.x += icon.size.x * Values::k;
		};
		buffer.forward(&assets::interface);
	};

	/// Draws health bar for a preview entity.
	void Bar::drawSquare(sf::IntRect area, ui::RenderBuffer& buffer) const {
		// get health bar texture
		sf::IntRect texture = Values::hp_bar(hp, max, poison);

		// get rendering position
		sf::IntRect draw = ui::DimRect({ 0.5as, 0.5as }, Values::tileSize).get(area);

		// draw health bar panel
		buffer.quad({
			{
				draw.position.x + (draw.size.x - texture.size.x * Values::k) / 2,
				draw.position.y + draw.size.y / 32
			},
			texture.size * Values::k
		}, texture);
		buffer.forward(&assets::tilemap);

		// get position of the first icon
		sf::Vector2i icon_pos = {
			draw.position.x + draw.size.x, draw.position.y
		};

		// draw effect icons
		for (EffectType effect : effects) {
			// draw icon
			sf::IntRect icon = Values::effect_textures[static_cast<int>(effect)];
			buffer.quad({
				icon_pos - sf::Vector2i(icon.size.x * Values::k, 0),
				icon.size * Values::k
			}, icon);

			// increment y
			icon_pos.y += icon.size.y * Values::k;
		};
		buffer.forward(&assets::interface);
	};

	/// Draws entity health bar.
	void entityBar(const Tile& tile, ui::RenderBuffer& buffer) {
		// select drawn entity
		const Entity* entity = nullptr;

		// fetch entity pointer
		if (tile.hex->troop) entity = &*tile.hex->troop;
		if (tile.hex->build) entity = &*tile.hex->build;		

		// ignore if no entity found
		if (!entity) return;

		// draw health bar
		Bar bar(*entity);
		if (!(tile.hex->build && bar.filled()))
			bar.drawAbove(tile.origin, buffer);
	};
};
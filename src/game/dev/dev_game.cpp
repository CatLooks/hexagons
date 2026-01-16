#include "dev/dev_game.hpp"
#include "game.hpp"
#include "mathext.hpp"
#include "flags.hpp"

namespace dev {
	/// Constructs a developer panel for the game.
	Panel* Factory::game_panel(Game* game) {
		// construct panel
		Panel* panel = new Panel(300px);

		// add debug mode display check
		game->onRecalculate([=](const sf::Time&) {
			if (flags::debug)
				panel->activate();
			else
				panel->deactivate();
		});
		
		// instantiate all sections
		attach_state(panel, game);
		attach_map(panel, game);
		attach_moves(panel, game);

		// return panel
		return panel;
	};

	/// Creates sections for game state.
	void Factory::attach_state(Panel* panel, Game* game) {
		// turn
		// player
		// team
		static const SectionLayout layout = {
			.title = "dp.state.title",
			.kv = {
				"dp.state.turn",
				"dp.state.player"
			}
		};
		auto* sec = panel->push();
		layout.construct(sec);

		// attach info update
		sec->attach([=]() {
			sec->args = {
				// general
				{ "turn", ext::str_int(game->_state.turn()) },
				{ "local", ext::str_int(game->_state._adapter->id) },

				// current player info
				{ "plr_now", ext::str_int(game->_state._idx) },
				{ "plr_count", ext::str_int(game->_state._plr.size()) },
				{ "plr_team", game->_state.player()
					? Values::hex_names[game->_state.player()->team]
					: "@!dp.null"
				}
			};
		});
	};

	/// Creates sections for map related stuff.
	void Factory::attach_map(Panel* panel, Game* game) {
		/// counters ///
		{
			static const SectionLayout layout = {
				.title = "dp.count.title",
				.kv = {
					"dp.count.hex",
					"dp.count.region",
					"dp.count.troop",
					"dp.count.build",
					"dp.count.plant"
				}
			};
			auto* sec = panel->push();
			layout.construct(sec);

			// attach info update
			sec->attach([=]() {
				sec->args = {
					{ "hex", ext::str_int(game->map.size().x * game->map.size().y) },

					// entity count
					{ "troop", ext::str_int(game->map._troops.count()) },
					{ "build", ext::str_int(game->map._builds.count()) },
					{ "plant", ext::str_int(game->map._plants.count()) },

					// entity pool count
					{ "troop_pool", ext::str_int(game->map._troops.capacity()) },
					{ "build_pool", ext::str_int(game->map._builds.capacity()) },
					{ "plant_pool", ext::str_int(game->map._plants.capacity()) },

					// region stuff
					{ "region", ext::str_int(game->map.regions._pool.count()) },
					{ "region_pool", ext::str_int(game->map.regions._pool.capacity()) }
				};
			});
		};
		/// map info ///
		{
			static const SectionLayout layout = {
				.title = "dp.map.title",
				.kv = {
					"dp.map.team",
					"dp.map.size_cursor",
					"dp.map.camera"
				}
			};
			auto* sec = panel->push();
			layout.construct(sec);

			// attach info update
			sec->attach([=]() {
				// convert mouse to coords
				sf::Vector2i coords = game->mouseToHex(game->_layer->map(
					ui::window.mouse(), { {}, ui::window.size() }
				));

				// set arguments
				sec->args = {
					{ "team", "all" },
					{ "size", ext::str_vec(game->map.size(), " x ") },
					{ "cursor", ext::str_vec(coords) },
					{ "camera", ext::str_vec(game->map.camera.position) },
					{ "zoom", ext::str_float(game->_camera.zoom(), 2) }
				};
			});
		};
		/// region info ///
		{
			static const SectionLayout layout = {
				.title = "dp.region.title",
				.kv = {
					"dp.region.id",
					"dp.region.count",
					"dp.region.money",
					"dp.region.fruit"
				}
			};
			auto* sec = panel->push([=]() { return game->map.selectedRegion(); });
			layout.construct(sec);

			// attach info update
			sec->attach([=]() {
				// get selected region
				const auto& reg = game->map.selectedRegion();
				if (!reg) return;

				// set arguments
				sec->args = {
					{ "id", ext::str_int(reg.index()) },
					{ "size", ext::str_int(reg->tiles) },
					{ "farm", ext::str_int(reg->farms) },
					{ "tent", ext::str_int(reg->tents) },

					// money
					{ "money", ext::str_int(reg->money) },
					{ "sign", reg->income == 0 ? "=" : (reg->income > 0 ? "+" : "-")},
					{ "income", ext::str_int(ext::iabs(reg->income)) },

					// fruits
					{ "berry", ext::str_int(reg->berry) },
					{ "peach", ext::str_int(reg->peach) },
				};
			});
		};
		/// hex info ///
		static HexRef tile = { nullptr, {} }; // hex capture pointer
		{
			static const SectionLayout layout = {
				.title = "dp.hex.title",
				.kv = {
					"dp.hex.general",
					"dp.hex.ids"
				}
			};
			auto* sec = panel->push([=]() {
				tile = game->_select
					? game->map.atref(*game->_select)
					: HexRef{ nullptr, {} };
				return tile.hex != nullptr;

			});
			layout.construct(sec);

			// attach info update
			sec->attach([=]() {
				if (!tile.hex) return;

				// set arguments
				sec->args = {
					{ "pos", ext::str_vec(tile.pos) },
					{ "team", Values::hex_names[tile.hex->team] },
					{ "select_id", ext::str_int(tile.hex->selected) },
					{ "spread_id", ext::str_int(tile.hex->spread) }
				};
			});
		};
		/// entity info ///
		static Entity* entity = nullptr; // entity capture pointer
		{
			static const SectionLayout layout = {
				.title = "dp.entity.title",
				.kv = {
					"dp.entity.values",
					"dp.entity.effects"
				}
			};
			auto* sec = panel->push([=]() {
				entity = tile.hex ? tile.hex->entity() : nullptr;
				return entity != nullptr;
			});
			layout.construct(sec);

			// attach info update
			sec->attach([=]() {
				if (!entity) return;

				// set arguments
				sec->args = {
					// hp
					{ "hp", ext::str_int(entity->hp) },
					{ "max", ext::str_int(entity->max_hp()) },

					// timers
					{ "t0", ext::str_int(entity->timers[0]) },
					{ "t1", ext::str_int(entity->timers[1]) },
					{ "t2", ext::str_int(entity->timers[2]) },
					{ "t3", ext::str_int(entity->timers[3]) },

					// effects
					{ "effects", Moves::str_eff(entity->effectList()) }
				};
			});
		};
		/// troop info ///
		static Troop* troop = nullptr; // troop capture pointer
		{
			static const SectionLayout layout = {
				.title = "dp.troop.title",
				.kv = {
					"dp.entity.type"
				}
			};
			auto* sec = panel->push([=]() {
				troop = (tile.hex && tile.hex->troop) ? &*tile.hex->troop : nullptr;
				return troop != nullptr;
			});
			layout.construct(sec);

			// attach info update
			sec->attach([=]() {
				if (!troop) return;

				// set arguments
				sec->args = {
					{ "id", ext::str_int((int)troop->type) },
					{ "type", Values::troop_names[troop->type] }
				};
			});
		};
		/// build info ///
		static Build* build = nullptr; // build capture pointer
		{
			static const SectionLayout layout = {
				.title = "dp.build.title",
				.kv = {
					"dp.entity.type"
				}
			};
			auto* sec = panel->push([=]() {
				build = (tile.hex && tile.hex->build) ? &*tile.hex->build : nullptr;
				return build != nullptr;
			});
			layout.construct(sec);

			// attach info update
			sec->attach([=]() {
				if (!build) return;

				// set arguments
				sec->args = {
					{ "id", ext::str_int((int)build->type) },
					{ "type", Values::build_names[build->type] }
				};
			});
		};
		/// plant info ///
		static Plant* plant = nullptr; // plant capture pointer
		{
			static const SectionLayout layout = {
				.title = "dp.plant.title",
				.kv = {
					"dp.entity.type",
					"dp.plant.fresh",
					"dp.plant.pity"
				}
			};
			auto* sec = panel->push([=]() {
				plant = (tile.hex && tile.hex->plant) ? &*tile.hex->plant : nullptr;
				return plant != nullptr;
			});
			layout.construct(sec);

			// attach info update
			sec->attach([=]() {
				if (!plant) return;

				// set arguments
				sec->args = {
					{ "id", ext::str_int((int)plant->type) },
					{ "type", Values::plant_names[plant->type] },

					{ "fresh", plant->fresh ? "@!yes" : "@!no" },
					{ "stage", ext::str_int(plant->stage) },
					{ "spread", ext::str_int(plant->spread) }
				};
			});
		};
	};

	/// Creates a section for all moves.
	void Factory::attach_moves(Panel* panel, Game* game) {
		/// move info ///
		static const ::Move* c_move = nullptr; // current move capture pointer
		static const ::Move* n_move = nullptr; // next move capture pointer

		// create sections
		auto* c_sec = panel->push([=]() {
			// fetch move object
			c_move = game->map.history.last();
			return c_move != nullptr;
		});
		auto* n_sec = panel->push([=]() {
			// fetch move object
			n_move = game->map.history.next();
			return n_move != nullptr;
		});

		// attaches a section constructor
		auto attach = [](Section* sec, const ::Move*& move, std::function<void(Section*)> title) {
			sec->attach([sec, title, &move]() {
				// ignore if no move
				if (!move) return;

				// clear section
				sec->clear();

				// add shared data
				title(sec);
				sec->line("dp.move.name.field", sf::Color::Cyan);
				sec->line("dp.move.shared");

				// set shared arguments
				sec->args = {
					{ "skill_type", Values::skill_names[move->skill_type] },
					{ "skill_pos", ext::str_vec(move->skill_pos) },
					{ "skill_time", ext::str_int(move->skill_cooldown) }
				};

				// add move specific info
				move->emitDev(sec, sec->args);
			});
		};

		// attach section constructors
		attach(c_sec, c_move, [](Section* sec) { sec->line("dp.move.last", sf::Color::Green); });
		attach(n_sec, n_move, [](Section* sec) { sec->line("dp.move.next", sf::Color::Red); });
	};
};
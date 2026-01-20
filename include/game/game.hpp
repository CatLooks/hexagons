#pragma once

// include dependencies
#include "ui/layer.hpp"
#include "ui/camera.hpp"
#include "ui/window.hpp"
#include "ui/anim/linear.hpp"

#include "ui/game_panel.hpp"
#include "ui/resource_bar.hpp"
#include "ui/hex_preview.hpp"
#include "ui/state_viewer.hpp"
#include "ui/splash_text.hpp"
#include "ui/progress_view.hpp"
#include "ui/chat.hpp"
#include "ui/editor.hpp"

#include "map.hpp"
#include "dev/dev_game.hpp"
#include <delegate>

namespace gameui { class Loader; };

/// Game controller object.
class Game : public ui::Element {
	friend dev::Factory;
	friend Editor;
	friend gameui::Loader;

public:
	Map map; /// Game map.

	/// Default tile position for deselection click.
	static const sf::Vector2i unselected;

private:
	sf::Vector2i    _last; /// Last clicked tile.
	ui::Layer*     _layer; /// Render layer.
	ui::Camera    _camera; /// Map camera.
	GameState&     _state; /// Current game state.
	gameui::Panel* _panel; /// UI control panel.
	gameui::Bar*     _bar; /// UI resource bar.
	gameui::State*  _view; /// UI game state viewer.
	gameui::Chat*   _chat; /// UI game chat.
	gameui::Progress* _pb; /// UI progress viewer.
	Editor*         _edit; /// UI editor.

	float _pulse = 0.f;      /// Map tile pulse.
	ui::Anim* _pulse_anim;   /// Pulse animation object.
	Delegate<void()> _queue; /// Update call queue.
	bool _move = false;      /// Whether the player is currenly making a move.

	/// Selected tile.
	std::optional<sf::Vector2i> _select;

public:
	gameui::Splash* splash; /// UI splash text.

	/// Queues a call for the next frame.
	/// 
	/// @param call Callback function.
	void queueCall(Delegate<void()>::Action call);

	uint8_t  skill_idx {}; /// Skill index (for skill timers).
	const Skill* skill {}; /// Current skill.
	SkillState   state {}; /// Skill state.

public:
	/// Constructs a game object.
	///
	/// @param game_layer Game rendering layer.
	/// @param ui_layer Game interface layer.
	/// @param chat_layer Game chat layer.
	/// @param state Game controller.
	Game(ui::Layer* game_layer, ui::Layer* ui_layer, ui::Layer* chat_layer, GameState* state);

	/// Undoes last move.
	void undoMove();
	/// Redoes last move.
	void redoMove();

	/// Selects a tile at position.
	/// 
	/// @param pos Tile position.
	void selectTile(sf::Vector2i pos);
	/// Deselects the tile.
	void deselectTile();
	
	/// Selects a region attached to a tile.
	///
	/// @param tile Hex reference.
	void selectRegion(const HexRef& tile);
	/// Deselects the region.
	void deselectRegion();

	/// Clicks at a tile.
	/// 
	/// @param pos Tile position.
	void click(sf::Vector2i pos);

	/// Cycles the building buying interface.
	void cycleBuild();
	/// Cycles the troop buying interface.
	void cycleTroop();

	/// Updates the building buying interface.
	void updateBuild() const;
	/// Updates the troop buying interface.
	void updateTroop() const;

	/// Deselect action buttons and cancels map selection.
	void deselectMenu();

	/// Constructs a region UI panel.
	/// 
	/// @param region Region object.
	/// @param targeted Whether the menu is targeted.
	void regionMenu(const Region& region, bool targeted = false);

	/// Constructs a troop UI panel.
	/// 
	/// @param troop Selected troop.
	void troopMenu(const Troop& troop);
	/// Constructs a building UI panel.
	/// 
	/// @param build Selected building.
	void buildMenu(const Build& build);
	/// Constructs a plant UI panel.
	/// 
	/// @param plant Selected plant.
	void plantMenu(const Plant& plant);
	/// Constructs a hex UI panel.
	/// 
	/// @param hex Selected hex.
	void hexMenu(const Hex& hex);

	/// Closes any open menus.
	void closeMenu();
	/// Updates menu state after a click.
	void updateMenu();

	/// Returns last click position.
	sf::Vector2i last() const;

	/// Whether local player is currently making a move.
	bool move() const;

	/// Returns hex coordinates at a mouse position.
	///
	/// @param mouse Mouse position.
	sf::Vector2i mouseToHex(sf::Vector2i mouse) const;

	/// Moves the camera to the middle of the map.
	void centerCamera();

protected:
	/// Draws the map.
	void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;
};
#pragma once

#include "ui.hpp"
#include "menu/mainMenu.hpp"
#include "menu/optionsMenu.hpp"
#include "menu/gameStartMenu.hpp"
#include "menu/gameJoinMenu.hpp"
#include "networking/Net.hpp"
#include <iostream>

class Game;
class GameState; 

/// Menu system controller.
///
/// Owns top-level menu pages and controls context switching between
/// the menu interface and the in-game interface.
class MenuSystem {
public:
    /// Menu UI context.
    ui::Interface::Context context;

    ui::Pages*     pages       = nullptr; /// Page container.

    MainMenu*      mainMenu    = nullptr; /// Main menu page.
    OptionsMenu*   optionsMenu = nullptr; /// Options menu page.
    GameStartMenu* startMenu   = nullptr; /// Game start page.
    GameJoinMenu*  joinMenu = nullptr; /// Game join page.

    /// Constructs a menu system.
    ///
    /// @param itf          UI interface.
    /// @param gameCtx      Game context id.
    /// @param gameInstance Game controller instance.
    /// @param net          Networking facade instance from main().
    MenuSystem(ui::Interface& itf, ui::Interface::Context* gameCtx, ui::Layer* pauseLayer, Game* gameInstance, Net& net, GameState& state);   
};

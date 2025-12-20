#pragma once

#include "ui.hpp"
#include "menu/mainMenu.hpp"
#include "menu/optionsMenu.hpp"
#include "menu/gameStartMenu.hpp"
#include <iostream>

class Game;

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

    /// Constructs a menu system.
    ///
    /// @param itf          UI interface.
    /// @param gameCtx      Game context id.
    /// @param gameInstance Game controller instance.
    MenuSystem(ui::Interface& itf, ui::Interface::Context* gameCtx, Game* gameInstance);
};
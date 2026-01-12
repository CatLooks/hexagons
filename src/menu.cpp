#include "menu.hpp"
#include "game.hpp"
// Include the new header
#include "menu/gameJoinMenu.hpp" 

MenuSystem::MenuSystem(ui::Interface& itf, ui::Interface::Context* gameCtx, Game* gameInstance)
    : context(itf.newContext())
{
    // switch to menu UI context
    itf.switchContext(context);

    // create root page container
    auto menu_layer = itf.layer();
    pages = new ui::Pages();
    pages->bounds = { 0, 0, 1ps, 1ps };
    menu_layer->add(pages);

    // construct pages
    mainMenu = new MainMenu();
    optionsMenu = new OptionsMenu();
    startMenu = new GameStartMenu();
    joinMenu = new GameJoinMenu();

	// add menus pages to container
    pages->add(mainMenu);
    pages->add(optionsMenu);
    pages->add(startMenu);
    pages->add(joinMenu);


    // show initial page
    pages->show(mainMenu);

	// bind login toggle page actions
    mainMenu->bindLogin([=]() {
        mainMenu->setLoggedIn(true);
        });

    mainMenu->bindLogout([=]() {
           mainMenu->setLoggedIn(false);
        });

    // main -> game start (Host/Singleplayer)
    mainMenu->bindStart([=]() {
        pages->show(startMenu);
        });

    // main -> join game
    mainMenu->bindJoin([=]() {
        pages->show(joinMenu);
        });

    // main -> options
    mainMenu->bindOptions([=]() {
        pages->show(optionsMenu);
        });

    // main -> exit
    mainMenu->bindExit([=]() {
        ui::window.close();
        });

    // options -> back
    optionsMenu->bindBack([=]() {
        pages->show(mainMenu);
        });

    // start menu -> back
    startMenu->bindBack([=]() {
        pages->show(mainMenu);
        });

    // start menu -> start game
    startMenu->bindStart([&itf, gameCtx, gameInstance]() {
        // Configure game based on startMenu selections here
        // e.g., gameInstance->configure(startMenu->getSelectedMode(), ...);

        gameInstance->activate();
        itf.switchContext(*gameCtx);
        });

    // join menu -> back
    joinMenu->bindBack([=]() {
        pages->show(mainMenu);
        });

    // join menu -> join action
    joinMenu->bindJoin([&itf, gameCtx, gameInstance](const std::string& code) {
        // 1. Pass the code to the game instance to initiate connection
        // gameInstance->connect(code); or smth like that?

		// 2. Switch to the game context - temp while no multiplayer implemented here
        gameInstance->activate();
        itf.switchContext(*gameCtx);
        });
}
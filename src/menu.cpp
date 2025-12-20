#include "menu.hpp"
#include "game.hpp"

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
    mainMenu    = new MainMenu();
    optionsMenu = new OptionsMenu();
    startMenu   = new GameStartMenu();

    pages->add(mainMenu);
    pages->add(optionsMenu);
    pages->add(startMenu);

    // show initial page
    pages->show(mainMenu);

    // main -> game start
    mainMenu->bindStart([=]() {
        pages->show(startMenu);
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
        gameInstance->activate();
        itf.switchContext(*gameCtx);
    });
}
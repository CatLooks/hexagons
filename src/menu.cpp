#include "menu.hpp"
#include "game.hpp"
#include "menu/gameJoinMenu.hpp"
#include "menu/ui/alertPopup.hpp"

MenuSystem::MenuSystem(ui::Interface& itf, ui::Interface::Context* gameCtx, Game* gameInstance, Net& net)
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
    startMenu = new GameStartMenu(&net);
    joinMenu = new GameJoinMenu(&net);

	// create global alert popup
    // Create the ONE Global Popup
    auto* globalAlert = new AlertPopup();
    
    // Add it to the layer AFTER 'pages' so it draws on top and catches clicks
    menu_layer->add(globalAlert);

    // Pass it to the menus that need it
    startMenu->setAlert(globalAlert);

     globalAlert->setBackground(pages); 
  
    // add menu pages to container
    pages->add(mainMenu);
    pages->add(optionsMenu);
    pages->add(startMenu);
    pages->add(joinMenu);


    // show initial page
    pages->show(mainMenu);


    // bind login toggle page actions
    mainMenu->bindLogin([=, &net]() {
        net.login();
        mainMenu->setLoggedIn(true);
        });

    mainMenu->bindLogout([=, &net]() {
        mainMenu->setLoggedIn(false);
        net.logout();
    });

    // main -> game start (Host/Singleplayer)
    mainMenu->bindStart([=]() {
        startMenu->reset(); //this will reset whenever getting to lobby (awaryjne)
        pages->show(startMenu);
        });

    // main -> join game
    mainMenu->bindJoin([=, &net]() {
        // CHECK: Prevent entering Join screen if not logged in

        if (!net.isLoggedIn()) {
            globalAlert->show("You must be logged in\nto join a Game!");
            return;
        }
        joinMenu->reset();
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


    // Options -> VSync Toggle
    optionsMenu->bindVSync([=](bool enabled) {
        ui::window.setVSync(enabled);
    });

    // Options -> Fullscreen Toggle
    optionsMenu->bindFullscreen([=](bool enabled) {
        ui::window.setFullscreen(enabled);
    });


    // start menu -> back
    startMenu->bindBack([=]() {
        pages->show(mainMenu);
        });

    // start menu -> start game
    startMenu->bindStart([&itf, gameCtx, gameInstance,this]() {
		const GameData& gameSettings = startMenu->getGameData(); // can be used to configure the game instance

        gameInstance->activate();
        itf.switchContext(*gameCtx);
        });

    // join menu -> back
    joinMenu->bindBack([=]() {
        joinMenu->reset(); 
        pages->show(mainMenu);
        });

    // join menu -> join action
    joinMenu->bindJoinSuccess([=](const std::string& code) {
        // 1. Configure the lobby menu as a client
        startMenu->enterAsJoiner(code);

        // 2. Show the lobby
        pages->show(startMenu);
    });   

	// po utworzeniu stron i pokazaniu mainMenu

	net.OnLobbyLeft.add([=]() {
        startMenu->reset(); 
		pages->show(mainMenu);
	});

	net.OnHostLobbyLeft.add([=]() {
        //globalAlert->show("The Host has ended the game.");
        startMenu->reset(); 
		pages->show(mainMenu);
	});

	// Exposed only: you will implement popup later
	net.OnLobbyDestroyed.add([=]() {
		// intentionally empty (popup + OK -> main menu handled by you)
        globalAlert->show("Lobby has been closed.");
        pages->show(mainMenu);
	});

     net.OnJoinFailed.add([=](const std::string& reason) {
        globalAlert->show("Failed to join lobby:\n" + reason);
        pages->show(mainMenu);
    });
}
#include "menu.hpp"
#include "game.hpp"
#include "menu/gameJoinMenu.hpp"
#include "menu/ui/alertPopup.hpp"
#include "game/sync/network_adapter.hpp" 
#include "game/serialize/map.hpp"

#include "game/sync/state.hpp" 

static void setup_test_map(Game& game) {
	Map& map = game.map;

	const int w = 18;
	const int h = 9;
	const char arr[h][w + 1] = {
		"vvv  rr     bb  rx",
		"rrvv   rrr  bbb  r",
		"vvv  rrbb   bb  rx",
		"rrvv   bbg       r",
		"vvv             rx",
		"rrvv rrr   oo    r",
		"vvv  ggg  ooo g rx",
		"rrvv ---   oo     ",
		"vvv              x"
	};

	map.empty({ w, h });

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Hex* hex = map.at({ x, y });
			if (hex == nullptr) continue;

			switch (arr[y][x]) {
			case '-':
				hex->type = Hex::Water;
				break;
			case 'r':
				hex->type = Hex::Ground;
				hex->team = Region::Red;
				break;
			case 'g':
				hex->type = Hex::Ground;
				hex->team = Region::Green;
				break;
			case 'b':
				hex->type = Hex::Ground;
				hex->team = Region::Blue;
				break;
			case 'y':
				hex->type = Hex::Ground;
				hex->team = Region::Yellow;
				break;
			case 'v':
				hex->type = Hex::Ground;
				hex->team = Region::Purple;
				break;
			case 'o':
				hex->type = Hex::Ground;
				hex->team = Region::Orange;
				break;
			case '#':
				hex->type = Hex::Ground;
				break;
			default:
				break;
			};
		};
	};

	map.regions.enumerate(&map);

	// damage test
	{
		Troop troop;
		troop.type = Troop::Knight;
		troop.hp = troop.max_hp();

		troop.pos = { 1, 1 };
		map.setTroop(troop);
		troop.pos = { 1, 3 };
		map.setTroop(troop);

		troop.addEffect(EffectType::OffenseBoost);

		troop.pos = { 1, 5 };
		map.setTroop(troop);
		troop.pos = { 1, 7 };
		map.setTroop(troop);
	};
	{
		Troop troop;
		troop.type = Troop::Baron;
		troop.hp = troop.max_hp();

		troop.pos = { 2, 1 };
		troop.hp++;
		map.setTroop(troop);
		troop.pos = { 2, 5 };
		troop.hp--;
		map.setTroop(troop);

		troop.addEffect(EffectType::DefenseBoost);

		troop.pos = { 2, 3 };
		map.setTroop(troop);
		troop.pos = { 2, 7 };
		map.setTroop(troop);
	};

	map.at({ 1, 4 })->region()->setRes({ 999, 999, 999 });
	map.at({ 0, 1 })->region()->setRes({ 999, 999, 999 });
	map.at({ 0, 3 })->region()->setRes({ 999, 999, 999 });
	map.at({ 0, 5 })->region()->setRes({ 999, 999, 999 });
	map.at({ 0, 7 })->region()->setRes({ 999, 999, 999 });

	// region split / merge test
	{
		Troop troop;
		troop.type = Troop::Farmer;
		troop.hp = troop.max_hp();

		troop.pos = { 9, 1 };
		map.setTroop(troop);

		troop.pos = { 8, 3 };
		map.setTroop(troop);
	};

	map.at({ 9, 1 })->region()->setRes({ 999, 999, 999 });
	map.at({ 8, 3 })->region()->setRes({ 999, 999, 999 });
	map.at({ 9, 3 })->region()->setRes({ 0, 6, 9 });

	// range test
	{
		Troop troop;
		troop.type = Troop::Archer;
		troop.hp = troop.max_hp();
		troop.pos = { 5, 5 };
		map.setTroop(troop);

		troop.type = Troop::Spearman;
		troop.hp = troop.max_hp();
		troop.pos = { 6, 6 };
		map.setTroop(troop);
		troop.pos = { 7, 6 };
		map.setTroop(troop);
	};

	map.at({ 5, 5 })->region()->setRes({ 999, 999, 999 });
	map.at({ 6, 6 })->region()->setRes({ 999, 999, 999 });

	// heal test
	{
		Build build;
		build.type = Build::Tent;
		build.pos = { 13, 1 };
		map.setBuild(build);

		Troop troop;
		troop.type = Troop::Knight;
		troop.hp = 4;
		troop.pos = { 12, 0 };
		map.setTroop(troop);

		troop.type = Troop::Spearman;
		troop.hp = 1;
		troop.pos = { 13, 0 };
		map.setTroop(troop);

		troop.type = Troop::Baron;
		troop.hp = 1;
		troop.pos = { 12, 2 };
		map.setTroop(troop);

		troop.type = Troop::Archer;
		troop.hp = troop.max_hp();
		troop.pos = { 13, 2 };
		map.setTroop(troop);
	};

	map.at({ 13, 1 })->region()->setRes({ 999, 999, 999 });

	// harvest test
	{
		Troop troop;
		troop.type = Troop::Farmer;
		troop.pos = { 11, 6 };
		map.setTroop(troop);

		Plant plant;
		plant.type = Plant::Peach;
		plant.pos = { 11, 5 };
		map.setPlant(plant);

		plant.type = Plant::Sapling;
		plant.pos = { 12, 5 };
		map.setPlant(plant);

		plant.type = Plant::Berry;
		plant.pos = { 11, 7 };
		map.setPlant(plant);

		plant.type = Plant::Bush;
		plant.pos = { 12, 7 };
		map.setPlant(plant);
	};

	map.at({ 11, 6 })->region()->setRes({ 999, 0, 0 });

	// misc tests
	{
		Troop troop;
		troop.type = Troop::Lumberjack;
		troop.pos = { 12, 6 };
		map.setTroop(troop);

		Build build;
		build.type = Build::Beacon;
		build.pos = { 1, 4 };
		build.hp = build.max_hp();
		map.setBuild(build);

		Plant plant;
		plant.type = Plant::Bush;
		plant.pos = { 16, 0 };
		map.setPlant(plant);
	};
}



MenuSystem::MenuSystem(ui::Interface& itf, ui::Interface::Context* gameCtx, Game* gameInstance, Net& net, GameState& state) : context(itf.newContext()) 
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
	startMenu->bindStart([&itf, gameCtx, gameInstance, this, &net, &state]() {
        
        // 1. Get Data (Populated correctly via fix in GameStartMenu::getGameData)
        const GameData& data = startMenu->getGameData();

        // 2. STOP listening to Lobby Packets immediately to prevent conflicts
        net.clearHandlers(); 

        // Helper: Map SFML Color to Game Team
		const std::vector<Region::Team> teamOrder = {
		Region::Red,    // Index 0 (Host)
		Region::Green,  // Index 1
		Region::Blue,   // Index 2
		Region::Yellow, // Index 3
		Region::Purple, // Index 4
			Region::Orange, // Index 5
			Region::Aqua    // Index 6
		};

        if (data.isMultiplayer) {
            // 3. Find My ID
            uint32_t myPlayerId = 0;
            bool found = false;
           
			printf("[Main] Local Name: '%s'\n", data.localPlayerName.c_str());

            for (size_t i = 0; i < data.players.size(); ++i) {
				printf("[Main] Checking list[%d]: '%s'\n", (int)i, data.players[i].name.c_str());
                // Compare names to find which index is "Me"
                if (data.players[i].name == data.localPlayerName) {
                    myPlayerId = (uint32_t)i;
                    found = true;
                    break;
                }
            }

            // Fallback if logic fails (e.g. testing with same account)
            if (!found) {
                printf("[CRITICAL] Could not find local player '%s' in player list!\n", data.localPlayerName.c_str());
                myPlayerId = startMenu->_isHost ? 0 : 1; 
            }

            printf("[Menu] Starting MP Game. My ID: %d\n", myPlayerId);

            // 4. Setup Adapter
            auto* adapter = new NetworkAdapter(net, myPlayerId);
            state.reset(
                startMenu->_isHost ? GameState::Host : GameState::Client, 
                adapter
            );

            // 5. Add Players
           for (size_t i = 0; i < data.players.size(); ++i) {
        // Pick team based on index (use % in case we have more players than colors)
        Region::Team assignedTeam = teamOrder[i % teamOrder.size()];
        
        state.addPlayer({ 
            .name = data.players[i].name, 
            .team = assignedTeam 
        });
        
        printf("[Main] Assigned %s to Team %d\n", data.players[i].name.c_str(), (int)assignedTeam);
    }

            // 6. Init Map
            if (startMenu->_isHost) {
                // Host loads the map logic locally
                setup_test_map(*gameInstance); 
                // Host sends the map Template to Clients via E_Init packet
                state.init(); 
            } else {
                // Client clears map and waits for E_Init packet from Host
                //gameInstance->map.clear();
            }

        } else {
            // Singleplayer Setup
            state.reset(GameState::Host, new TestAdapter());
            state.addPlayer({ .name = "Player", .team = Region::Red });
            state.addPlayer({ .name = "Bot", .team = Region::Blue });
            
            setup_test_map(*gameInstance);
            state.init();
        }

        // Switch context to Game
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
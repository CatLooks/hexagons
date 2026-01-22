#include "menu.hpp"
#include "game.hpp"
#include "menu/gameJoinMenu.hpp"
#include "menu/ui/alertPopup.hpp"
#include "game/sync/network_adapter.hpp"
#include "game/serialize/map.hpp"
#include "game/loader.hpp" 
#include "game/sync/state.hpp"
#include "game/bot_ai.hpp"

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



MenuSystem::MenuSystem(ui::Interface& itf, ui::Interface::Context* gameCtx, ui::Interface::Context* editCtx, Game* gameInstance, Net& net, GameState& state) : context(itf.newContext())
{
    // switch to menu UI context
    itf.switchContext(context);

    // create root page container
    auto menu_layer = itf.layer();
    pages = new ui::Pages();
    pages->bounds = { 0, 0, 1ps, 1ps };
    menu_layer->add(pages);

    // add exit callback
    menu_layer->add(new ui::Exit([=]() {
    	ui::window.close();
    }, 0px));

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
            globalAlert->show(assets::lang::locale.req("menu.login_required").get({}));            
            return;
        }
        joinMenu->reset();
        pages->show(joinMenu);
    }); 

    // main -> editor
    mainMenu->bindEdit([=, &itf]() {
        itf.switchContext(*editCtx);
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
    
    // 1. Get Data from the menu
    const GameData& data = startMenu->getGameData();

    // 2. STOP listening to Lobby Packets
    net.clearHandlers(); 

    // Define Team Order
    const std::vector<Region::Team> teamOrder = {
        Region::Red, Region::Green, Region::Blue, Region::Yellow, 
        Region::Purple, Region::Orange, Region::Aqua
    };

    // --- A. NETWORK/ADAPTER SETUP ---
    if (data.isMultiplayer) {
        // --- MULTIPLAYER SETUP ---
        
        // Find My ID based on local name
        uint32_t myPlayerId = 0;
        bool found = false;
        
        for (size_t i = 0; i < data.players.size(); ++i) {
            if (data.players[i].name == data.localPlayerName) {
                myPlayerId = (uint32_t)i;
                found = true;
                break;
            }
        }
        
        if (!found) {
            printf("[Main] Warning: Could not find local player in list. Defaulting ID.\n");
            myPlayerId = startMenu->_isHost ? 0 : 1; 
        }

        printf("[Main] Starting MP Game. My ID: %d\n", myPlayerId);

        // Create Network Adapter
        auto* adapter = new NetworkAdapter(net, myPlayerId);
        state.reset(
            startMenu->_isHost ? GameState::Host : GameState::Client, 
            adapter
        );

        // Add Players to State
        for (size_t i = 0; i < data.players.size(); ++i) {
            state.addPlayer({ 
                .name = data.players[i].name, 
                .team = teamOrder[i % teamOrder.size()] 
            });
        }
    } 
    else {
        // --- SINGLEPLAYER SETUP ---
        
        // Create Local Test Adapter
        float difficulty = 0.f;
        switch (data.difficulty) {
            case GameData::Difficulty::Easy: difficulty = 0.0f; break;
            case GameData::Difficulty::Medium: difficulty = 0.5f; break;
            case GameData::Difficulty::Hard: difficulty = 1.0f; break;
        };
        auto* adapter = new BotAdapter(difficulty);
        adapter->map = &gameInstance->map;
        state.reset(GameState::Host, adapter);

        // Add default players
        //state.addPlayer({ .name = "Player", .team = Region::Red });
        //state.addPlayer({ .name = "Bot", .team = Region::Blue });
    }

    // --- B. MAP LOADING LOGIC ---
    
    // Only the Authority (Host or Singleplayer) loads the map from disk.
    // Clients do NOTHING here; they receive the map via network in state.init().
    if (startMenu->_isHost || !data.isMultiplayer) {
        
        bool mapLoaded = false;

        // 1. Check if a valid map file was selected in the menu
        if (!data.selectedMapPath.empty() && data.selectedMapPath != "STATIC_MAP_ID") {
            printf("[Main] Attempting to load map: %s\n", data.selectedMapPath.c_str());
            
            // 2. Use the Loader to read the file
            auto fileInfo = Loader::load(data.selectedMapPath);
            
            if (fileInfo.has_value()) {
                // 3. Clear existing data and construct the new map
                gameInstance->map.clear();
                fileInfo->temp.construct(&gameInstance->map);

                // add bot players
                if (!data.isMultiplayer) {
                    auto teams = ai::teams(gameInstance->map, Region::Red);
                    for (Region::Team team : teams) {
                        state.addPlayer({
                            .name = team == Region::Red
                                ? (data.localPlayerName.empty()
                                    ? assets::lang::locale.req("gp.player_default").get({})
                                    : data.localPlayerName)
                                : assets::lang::locale.req("gp.bot").get({}),
                            .team = team
                        });
                    };
                };
                
                // 4. Center camera and flag success
                gameInstance->centerCamera();
                mapLoaded = true;
                printf("[Main] Map loaded successfully.\n");
            } else {
                printf("[Main] Failed to load map file!\n");
            }
        }

        // 5. Fallback: If no file selected or load failed, use the hardcoded test map
        if (!mapLoaded) {
            printf("[Main] Using fallback hardcoded map.\n");
            setup_test_map(*gameInstance);
        }

        // 6. Initialize State 
        // (If Host: Serializes map to send to clients. If Singleplayer: Just starts.)
        state.init(); 
    } 
    else {
        // --- CLIENT LOGIC ---
        // Clients wait for the 'E_Init' packet from the Host, 
        // which contains the map data loaded above.
    }

    // --- C. SWITCH TO GAME ---
    gameInstance->activate();
    itf.switchContext(*gameCtx);

    // add exit to game
    gameInstance->add(new ui::Exit([&]() {
    	itf.switchContext(context);
    	pages->show(mainMenu);
    }, 120px));
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
        globalAlert->show(assets::lang::locale.req("lobby.host_ended").get({}));
        startMenu->reset(); 
		pages->show(mainMenu);
	});

	// Exposed only: you will implement popup later
	net.OnLobbyDestroyed.add([=]() {
		// intentionally empty (popup + OK -> main menu handled by you)
        globalAlert->show(assets::lang::locale.req("lobby.closed").get({}));
        pages->show(mainMenu);
	});

     net.OnJoinFailed.add([=](const std::string& reason) {
         globalAlert->show(assets::lang::locale.req("lobby.join_failed").get({
            { "reason", reason }
         }));
         pages->show(mainMenu);
    });
}
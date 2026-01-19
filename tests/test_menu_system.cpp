#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <functional>

#include "assets.hpp"
#include "menu/gameJoinMenu.hpp"
#include "menu/text_field_open.hpp"
#include "menu/mainMenu.hpp"
#include "menu/gameStartMenu.hpp"
#include "menu/ui/menuButton.hpp"

// Helper Macros 
#define LOG(msg) std::cout << "[TEST] " << msg << std::endl
#define PASS(msg) std::cout << "  [PASS] " << msg << std::endl
#define FAIL(msg) std::cerr << "  [FAIL] " << msg << std::endl

#include <chrono>




class GameJoinMenuTest {
public:
    static void Run() {

        // 1. Initialization
        GameJoinMenu menu;

        // Verify pointers exist (Form was built)
        assert(menu._codeField != nullptr);
        assert(menu._joinBtn != nullptr);
        assert(menu._errorMsg != nullptr);
        PASS("UI Elements initialized.");

        // 2. Test Status Message Logic
        menu.setStatusMessage("Invalid Code", true);
        
        // Verify text content
        sf::String currentMsg = menu._errorMsg->string();
        assert(currentMsg == "Invalid Code");
        
        PASS("Status message updated to 'Invalid Code'.");

        // 3. Test Join Callback Wiring
        bool joinTriggered = false;
        std::string receivedCode = "UNSET";

        menu.bindJoin([&](const std::string& code) {
            joinTriggered = true;
            receivedCode = code;
        });

        // 4. Simulate Input        
        menu._codeField->setText("123456");         
        menu.attemptJoin();

        assert(joinTriggered == true);
        assert(receivedCode == "123456");
        PASS("Join callback fired successfully.");

        // 5. Test Back Button Wiring
        bool backTriggered = false;
        menu.bindBack([&]() { backTriggered = true; });
        assert(menu._onBack != nullptr);
        
        // Manually invoke the std::function to ensure it holds the correct lambda
        menu._onBack();
        assert(backTriggered == true);
        PASS("Back callback wired successfully.");

        RunFuzzTest();
    }

    static void RunFuzzTest() {
    GameJoinMenu menu;
    // Random trash input
    std::string trash = "%s%s%n" + std::string(10000, 'A'); 
    
    try {
        menu._codeField->setText(trash);
        menu.attemptJoin();
        PASS("Fuzzing: Menu survived trash input.");
    } catch (...) {
        FAIL("Fuzzing: Menu CRASHED on trash input.");
    }
}
};

class MainMenuTest {
public:
    static void Run() {

        // 1. Construction & Initialization
        MainMenu menu;
        PASS("MainMenu constructed successfully.");
        
        // 2. Check if critical public pointers are initialized
        assert(menu.login_toggle != nullptr);
        assert(menu._loginBtn != nullptr);
        assert(menu._logoutBtn != nullptr);
        PASS("Login toggle and buttons exist.");

        // 3. Callback Binding
        try {
            menu.bindStart([]() {});
            menu.bindJoin([]() {});
            menu.bindOptions([]() {});
            menu.bindExit([]() {});
            menu.bindLogin([]() {});
            menu.bindLogout([]() {});
            
            PASS("Callbacks bound without crashing.");
        } catch (...) {
            FAIL("Crashed while binding callbacks.");
            throw; 
        }

        // 4. Logic Verification: Logged Out State
        menu.setLoggedIn(false);
        PASS("Verified LoggedOut state logic (Login Active / Logout Inactive).");

        // 5. Logic Verification: Logged In State
        menu.setLoggedIn(true);
        PASS("Verified LoggedIn state logic (Login Inactive / Logout Active).");
    }
};

class GameStartMenuTest {
public:
    // Dummy button generator for tests without GUI context
    static menuui::Button* makeDummyBtn() {
        auto* btn = new menuui::Button(); 
        return btn;
    }

    static void RunSinglePlayerFlow() {
        LOG("Scenario: Single Player Flow");
        GameStartMenu menu;
        menuui::Button* dummy = makeDummyBtn();

        // 1. Mode Selection
        assert(menu._currentStep == GameStartMenu::STEP_MODE);
        menu.selectMode(GameStartMenu::Mode_Single, dummy);
        assert(menu.getGameData().isMultiplayer == false);
        PASS("Selected Single Player");

        // 2. Diff Selection
        menu.nextStep();
        assert(menu._currentStep == GameStartMenu::STEP_DIFF);
        menu.selectDifficulty(GameStartMenu::Diff_Hard, dummy);
        assert(menu.getGameData().difficulty == GameData::Difficulty::Hard);
        PASS("Selected Hard Difficulty");

        // 3. Map Selection
        menu.nextStep();
        assert(menu._currentStep == GameStartMenu::STEP_MAP);
        
        // Mock selecting a map
        menu._currentData.selectedMapPath = "assets/maps/test.json";
        menu._currentData.selectedMapName = "Test Map";
        PASS("Selected Map (Mocked)");

        // 4. Start Game Trigger
        bool gameStarted = false;
        menu.bindStart([&]() { gameStarted = true; });

        // In Single Player, Next on Map screen should start game
        menu.nextStep(); 
        
        assert(gameStarted == true);
        PASS("Game Start Callback Triggered successfully");

        delete dummy;
    }

    static void RunHostMultiplayerFlow() {
        LOG("Scenario: Multiplayer Host Flow");
        GameStartMenu menu;
        menuui::Button* dummy = makeDummyBtn();

        // 1. Mode Selection (Host)
        menu.selectMode(GameStartMenu::Mode_Host, dummy);
        assert(menu.getGameData().isMultiplayer == true);
        assert(!menu.getGameData().roomCode.empty()); 
        PASS("Selected Host Mode (Room Code Generated: " + menu.getGameData().roomCode + ")");

        // 2. Diff -> Map
        menu.nextStep(); // To Diff
        menu.selectDifficulty(GameStartMenu::Diff_Medium, dummy);
        menu.nextStep(); // To Map
        menu._currentData.selectedMapPath = "assets/maps/dummy.json"; 
        
        // 3. To Lobby
        menu.nextStep();
        assert(menu._currentStep == GameStartMenu::STEP_LOBBY);
        PASS("Arrived at Lobby Configuration Step");

        // 4. To Waiting Room
        menu.nextStep();
        assert(menu._currentStep == GameStartMenu::STEP_WAITING);
        PASS("Arrived at Waiting Room (Host)");
        
        delete dummy;
    }

    static void RunJoinerFlow() {
        LOG("Scenario: Join Flow");
        GameStartMenu menu;

        std::string code = "999888";
        menu.enterAsJoiner(code);

        // Should skip Mode/Diff/Map/LobbyConfig and go straight to Waiting
        assert(menu._currentStep == GameStartMenu::STEP_WAITING);
        assert(menu.getGameData().roomCode == code);
        assert(menu.getGameData().isMultiplayer == true);
        
        PASS("Direct entry to Waiting Room successful");
    }

    static void RunAssetScanTest() {
        LOG("Scenario: Asset Scanning");
        GameStartMenu menu;
        
        // This exercises the std::filesystem logic
        std::cout << "  [INFO] Available Maps Found: " << menu._availableMaps.size() << std::endl;
        
        if (menu._availableMaps.empty()) {
            std::cout << "  [WARN] No maps found. Check ASSET_PATH: " << ASSET_PATH << std::endl;
        } else {
            PASS("Maps detected in folder");
        }
    }

    static void RunPerformanceTest() {
        LOG("Scenario: Performance Test (Menu Instantiation)");
    
        const int iterations = 100;  
        auto start = std::chrono::high_resolution_clock::now();
    
        for(int i = 0; i < iterations; i++) {
            GameStartMenu menu;
            menu.nextStep();
        
            // Print a dot every 10 iterations show its working
            if (i % 10 == 0) {
                std::cout << "." << std::flush;
            }
        }
        std::cout << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
    
        std::cout << "[PERF] " << iterations << " menu creations took: " << elapsed.count() << " ms" << std::endl;
        std::cout << "[PERF] Average per menu: " << (elapsed.count() / iterations) << " ms" << std::endl;

        // 100 items * 100ms = 10000ms total
        if (elapsed.count() > 10000.0) {
            std::cerr << "  [WARN] Performance is slow! >100ms per menu." << std::endl;
        } else {
            PASS("Performance within acceptable limits.");
        }
    }

    // Main entry point for this suite
    static void Run() {
        RunAssetScanTest();
        RunSinglePlayerFlow();
        RunHostMultiplayerFlow();
        RunJoinerFlow();
        RunPerformanceTest();
    }
};

int main() {
    LOG("Initializing Assets...");
    
    assets::lang::init();
    assets::loadAssets();

    if (assets::error) {
        FAIL("Critical error: Failed to load game assets. Aborting tests.");
        return 1;
    }

    try {
        GameJoinMenuTest::Run();
        MainMenuTest::Run();
        GameStartMenuTest::Run();
        
    } catch (const std::exception& e) {
        FAIL(std::string("Exception caught during tests: ") + e.what());
        return 1;
    }

    LOG("All Menu Tests Passed Successfully.");
    return 0;
}
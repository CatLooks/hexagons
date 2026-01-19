#pragma once
#include "ui/layer.hpp"
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "ui/pages.hpp" 
#include "menu/ui/menuButton.hpp"
#include "networking/Net.hpp"
#include <functional>
#include <vector>
#include <string>
#include "menu/lobbyMenu.hpp"

class Net;

/// Game start configuration menu.
class GameStartMenu : public ui::Element {
public:
    /// Game start menu callback type.
    using Action = std::function<void()>;

    /// Game mode value.
    enum GameMode { Mode_None = -1, Mode_Single, Mode_Host };
    /// Difficulty value.
    enum Difficulty { Diff_None = -1, Diff_Easy, Diff_Medium, Diff_Hard };

    // Steps
    enum Step {
        STEP_MODE,
        STEP_DIFF,
        STEP_MAP,
        STEP_LOBBY,
        STEP_WAITING 
    };

    /// Main pages switcher for setup views.
    ui::Pages* _mainSwitcher;    /// Main pages switcher.
    /// Container for sidebar, content pages and navigation area.
    ui::Element* _setupContainer; /// Setup container.

private:
    Net* _net = nullptr;
    bool _isHost = false;

    ui::Solid* _bg = nullptr; /// Background panel.
    ui::Solid* _sidebarBg = nullptr; /// Sidebar background panel.
    ui::Pages* _contentPages = nullptr; /// Content page container.

    // Sidebar labels
    ui::Text* _lblMode = nullptr;    /// Mode label in sidebar.
    ui::Text* _lblDiff = nullptr;    /// Difficulty label in sidebar.
    ui::Text* _lblMap = nullptr;     /// Map label in sidebar.
    ui::Text* _lblLobby = nullptr;   /// Lobby label in sidebar (multiplayer).

    ui::Element* _navArea = nullptr; /// Navigation area element.

    menuui::Button* _backBtn = nullptr; /// Back navigation button.
    menuui::Button* _prevBtn = nullptr; /// Previous step button.
    menuui::Button* _nextBtn = nullptr; /// Next / start button.

    ui::Element* _pageMode = nullptr;   /// Mode selection page root.
    ui::Element* _pageDiff = nullptr;   /// Difficulty selection page root.
    ui::Element* _pageMap = nullptr;    /// Map selection page root.
    ui::Element* _pageLobby = nullptr;  /// Lobby settings page root.
    LobbyMenu* _pageWaitingLobby;       /// Waiting lobby page root.

    // Page title labels (stored to update localization on language change)
    ui::Text* _pageModeTitle = nullptr;
    ui::Text* _pageDiffTitle = nullptr;
    ui::Text* _pageMapTitle = nullptr;
    ui::Text* _pageLobbyTitle = nullptr;

    // Lobby page labels
    ui::Text* _lblMaxPlayers = nullptr;
    ui::Text* _lblRoomCodeDesc = nullptr;
    ui::Text* _lblRoomCode = nullptr;

	std::string _scanDiagnostic;    /// Diagnostic message from map scanning.
    int _currentStep = 0;           /// Currently active configuration step.
	bool _hasSelectedMode = false;  /// Whether a mode has been selected.
	bool _hasSelectedDiff = false;  /// Whether a difficulty has been selected.

	GameData _currentData; /// Current game data.

    // UI Item Containers
    std::vector<menuui::Button*> _modeButtons;         /// Mode buttons.
    std::vector<menuui::Button*> _playerCountButtons;  /// Player count buttons.
    std::vector<menuui::Button*> _diffButtons;         /// Difficulty buttons.
    std::vector<menuui::Button*> _mapButtons;          /// Map buttons.
  
    Action _onBack;      /// Back callback.
    Action _onStartGame; /// Start game callback.
    
    std::vector<MapInfo> _availableMaps;
    int _mapPageOffset = 0; // Index of the first map shown on the current page
    
    menuui::Button* _mapPrevBtn = nullptr;
    menuui::Button* _mapNextBtn = nullptr;
    ui::Element* _mapGrid = nullptr;

public:
    /// Constructs a game start menu.
    GameStartMenu(Net* net);

    /// Binds back button callback.
    void bindBack(Action action);
    /// Binds start button callback.
    void bindStart(Action action);

	/// @return Current game data.
    const GameData& getGameData() const { return _currentData; }
	/// Sets current game data.
    void setGameData(const GameData& data) { _currentData = data; updateUI(); }

    /// @return The max players (returns 1 if singleplayer).
    int getMaxPlayers() const { return (_currentData.isMultiplayer) ? _currentData.maxPlayers: 1; }
    /// @return The generated game code.
    std::string getGameCode() const { return _currentData.roomCode; }
    void enterAsJoiner(const std::string& code);

protected:
    void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;

private:
    void buildSidebar();
    void buildNavigation();

    ui::Element* createModePage();
    ui::Element* createDiffPage();
    ui::Element* createMapPage();
    ui::Element* createLobbyPage();

    void selectMode(GameMode mode, menuui::Button* btn);
    void selectDifficulty(Difficulty diff, menuui::Button* btn);
    void selectMaxPlayers(int count, menuui::Button* btn);

    void generateGameCode();

    bool canProceed() const;
    void nextStep();
    void prevStep();

    void updateUI();
    void updateNavigationButtons();
    void updateSidebarLabels(); /// Helper for dynamic sidebar

	void refreshAllText();

    void scanMaps();                /// Scans the folder (should probably be moved to assets loading)
    void updateMapGrid();           /// Refreshes the 3 visible maps
	void changeMapPage(int delta);  /// Changes the map page by delta

    void addSelfToUI();
    
    // Packet Protocol
    enum PacketType { Pkt_Hello = 100, Pkt_LobbyState = 101, Pkt_StartGame = 102 };

    struct LobbyMember {
        std::string netId;
        PlayerData data;
    };
    std::vector<LobbyMember> _connectedMembers;

    // Helper to setup listeners
    void bindNetworkHandlers();
    
    // Logic handlers
    void onPlayerJoined(const std::string& id);
    void onPlayerLeft(const std::string& id);
    void onPacket(const std::string& id, sf::Packet& pkt);

    // Sync helpers
    void sendHello();
    void broadcastLobbyState();

    std::string _localPlayerName;   /// The name this client generated for themselves.
    bool _acknowledgedByHost = false; /// If the host has sent back our name in the lobby state.
    sf::Clock _heartbeatTimer;      /// Timer for resending Hello packets.
};
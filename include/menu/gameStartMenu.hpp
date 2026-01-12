#pragma once
#include "ui/layer.hpp"
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "ui/pages.hpp" 
#include "menu/ui/menuButton.hpp"
#include <functional>
#include <vector>
#include <string>
#include "menu/lobbyMenu.hpp"

/// Game start configuration menu.
class GameStartMenu : public ui::Element {
public:
    /// Game start menu callback type.
    using Action = std::function<void()>;

    /// Game mode value.
    enum GameMode { Mode_None = -1, Mode_Single, Mode_Host };
    /// Difficulty value.
    enum Difficulty { Diff_None = -1, Diff_Easy, Diff_Medium, Diff_Hard };
    /// Map identifier.
    enum MapID { Map_None = -1, Map_1, Map_2, Map_3 };

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

    ui::Element* _pageMode = nullptr; /// Mode selection page root.
    ui::Element* _pageDiff = nullptr; /// Difficulty selection page root.
    ui::Element* _pageMap = nullptr;  /// Map selection page root.
    ui::Element* _pageLobby = nullptr; /// Lobby settings page root.
    LobbyMenu* _pageWaitingLobby;      /// Waiting lobby page root.

    int _currentStep = 0; /// Currently active configuration step.

    // Selection State
    GameMode   _selectedMode = Mode_None;   /// Selected game mode.
    Difficulty _selectedDiff = Diff_None;   /// Selected difficulty.
    MapID      _selectedMap = Map_None;     /// Selected map id.

    // Multiplayer State
    int         _maxPlayers = 2; /// Maximum players for host.
    std::string _gameCode;      /// Generated game code.

    // UI Item Containers
    std::vector<menuui::Button*> _modeButtons;         /// Mode buttons.
    std::vector<menuui::Button*> _playerCountButtons;  /// Player count buttons.
    std::vector<menuui::Button*> _diffButtons;         /// Difficulty buttons.
    std::vector<menuui::Button*> _mapButtons;          /// Map buttons.
  
    Action _onBack;      /// Back callback.
    Action _onStartGame; /// Start game callback.

public:
    /// Constructs a game start menu.
    GameStartMenu();

    /// Binds back button callback.
    void bindBack(Action action);
    /// Binds start button callback.
    void bindStart(Action action);

    GameMode getSelectedMode() const { return _selectedMode; }
    Difficulty getSelectedDifficulty() const { return _selectedDiff; }
    MapID getSelectedMap() const { return _selectedMap; }

    /// @return The max players (returns 1 if singleplayer).
    int getMaxPlayers() const { return (_selectedMode == Mode_Host) ? _maxPlayers : 1; }
    /// @return The generated game code.
    std::string getGameCode() const { return _gameCode; }
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
    void selectMap(MapID map, menuui::Button* btn);
    void selectMaxPlayers(int count, menuui::Button* btn);

    void generateGameCode();

    bool canProceed() const;
    void nextStep();
    void prevStep();

    void updateUI();
    void updateNavigationButtons();
    void updateSidebarLabels(); /// Helper for dynamic sidebar
};
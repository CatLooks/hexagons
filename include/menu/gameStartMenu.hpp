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

    // Step constants
    static constexpr int STEP_MODE = 0;
    static constexpr int STEP_DIFF = 1;
    static constexpr int STEP_MAP = 2;
    static constexpr int STEP_LOBBY = 3; // Only for Multiplayer

private:
    ui::Solid* _bg = nullptr; /// Background panel.
    ui::Solid* _sidebarBg = nullptr; /// Sidebar background panel.
    ui::Pages* _contentPages = nullptr; /// Content page container.

    // Sidebar labels
    ui::Text* _lblMode = nullptr;
    ui::Text* _lblDiff = nullptr;
    ui::Text* _lblMap = nullptr;
    ui::Text* _lblLobby = nullptr; // New: Multiplayer settings

    menuui::Button* _backBtn = nullptr; /// Back navigation button.
    menuui::Button* _prevBtn = nullptr; /// Previous step button.
    menuui::Button* _nextBtn = nullptr; /// Next / start button.

    ui::Element* _pageMode = nullptr; /// Mode selection page root.
    ui::Element* _pageDiff = nullptr; /// Difficulty selection page root.
    ui::Element* _pageMap = nullptr; /// Map selection page root.
    ui::Element* _pageLobby = nullptr; /// Lobby settings page root.

    int _currentStep = 0; /// Currently active configuration step.

    // Selection State
    GameMode   _selectedMode = Mode_None;
    Difficulty _selectedDiff = Diff_None;
    MapID      _selectedMap = Map_None;

    // Multiplayer State
    int         _maxPlayers = 2;
    std::string _gameCode;

    // UI Item Containers
    std::vector<menuui::Button*> _modeButtons;
    std::vector<menuui::Button*> _playerCountButtons;
    std::vector<menuui::Button*> _diffButtons;
    std::vector<menuui::Button*> _mapButtons;

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

protected:
    void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;

private:
    void buildSidebar();
    void buildNavigation();

    ui::Element* createModePage();
    ui::Element* createDiffPage();
    ui::Element* createMapPage();
    ui::Element* createLobbyPage(); // New

    void selectMode(GameMode mode, menuui::Button* btn);
    void selectDifficulty(Difficulty diff, menuui::Button* btn);
    void selectMap(MapID map, menuui::Button* btn);
    void selectMaxPlayers(int count, menuui::Button* btn); // New

    void generateGameCode(); // New

    bool canProceed() const;
    void nextStep();
    void prevStep();

    void updateUI();
    void updateNavigationButtons();
    void updateSidebarLabels(); // Helper for dynamic sidebar
};
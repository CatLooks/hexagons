#pragma once
#include "ui/layer.hpp"
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "ui/pages.hpp" 
#include "menu/ui/menuButton.hpp"
#include <functional>
#include <vector>

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

private:
    ui::Solid* _bg           = nullptr; /// Background panel.
    ui::Solid* _sidebarBg    = nullptr; /// Sidebar background panel.
    ui::Pages* _contentPages = nullptr; /// Content page container.

    ui::Text* _lblMode = nullptr; /// Sidebar mode step label.
    ui::Text* _lblDiff = nullptr; /// Sidebar difficulty step label.
    ui::Text* _lblMap  = nullptr; /// Sidebar map step label.

    menuui::Button* _backBtn = nullptr; /// Back navigation button.
    menuui::Button* _prevBtn = nullptr; /// Previous step button.
    menuui::Button* _nextBtn = nullptr; /// Next / start button.

    ui::Element* _pageMode = nullptr; /// Mode selection page root.
    ui::Element* _pageDiff = nullptr; /// Difficulty selection page root.
    ui::Element* _pageMap  = nullptr; /// Map selection page root.

    int _currentStep = 0; /// Currently active configuration step.

    GameMode   _selectedMode = Mode_None;  /// Selected game mode.
    Difficulty _selectedDiff = Diff_None;  /// Selected difficulty.
    MapID      _selectedMap  = Map_None;   /// Selected map id.

    std::vector<menuui::Button*> _modeButtons; /// Mode selection buttons.
    std::vector<menuui::Button*> _diffButtons; /// Difficulty selection buttons.
    std::vector<menuui::Button*> _mapButtons;  /// Map selection buttons.

    Action _onBack;      /// Back callback.
    Action _onStartGame; /// Start game callback.

public:
    /// Constructs a game start menu.
    GameStartMenu();

    /// Binds back button callback.
    /// @param action Callback invoked when the back button is pressed.
    void bindBack(Action action);
    /// Binds start button callback.
    /// @param action Callback invoked when the start button is pressed.
    void bindStart(Action action);

    /// @return Selected game mode.
    GameMode getSelectedMode() const { return _selectedMode; }
    /// @return Selected difficulty.
    Difficulty getSelectedDifficulty() const { return _selectedDiff; }
    /// @return Selected map id.
    MapID getSelectedMap() const { return _selectedMap; }

protected:
    /// Draws the game start menu.
    /// @param target Render buffer.
    /// @param self   Element drawing area.
    void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;

private:
    /// Builds sidebar UI elements.
    void buildSidebar();
    /// Builds navigation controls.
    void buildNavigation();

    /// Creates game mode selection page.
    ui::Element* createModePage();
    /// Creates difficulty selection page.
    ui::Element* createDiffPage();
    /// Creates map selection page.
    ui::Element* createMapPage();

    /// Selects a game mode.
    /// @param mode Mode value.
    /// @param btn  Source button.
    void selectMode(GameMode mode, menuui::Button* btn);
    /// Selects a difficulty.
    /// @param diff Difficulty value.
    /// @param btn  Source button.
    void selectDifficulty(Difficulty diff, menuui::Button* btn);
    /// Selects a map.
    /// @param map Map id.
    /// @param btn Source button.
    void selectMap(MapID map, menuui::Button* btn);

    /// @return Whether the current step has valid selection.
    bool canProceed() const;
    /// Advances to the next configuration step.
    void nextStep();
    /// Returns to the previous configuration step.
    void prevStep();

    /// Updates all UI according to current state.
    void updateUI();
    /// Updates navigation button visibility and labels.
    void updateNavigationButtons();
};
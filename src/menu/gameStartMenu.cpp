#include "menu/gameStartMenu.hpp"
#include "assets.hpp"
#include <random>
#include <iomanip>
#include <sstream>

/// Header text settings.
static const ui::TextSettings k_HeaderFont = {
    assets::font, 60, sf::Color::White, sf::Color::Black, 3
};
/// Sidebar text settings.
static const ui::TextSettings k_SidebarFont = {
    assets::font, 30, sf::Color(150, 150, 150), sf::Color::Black, 0
};
/// Active sidebar text settings.
static const ui::TextSettings k_SidebarFontActive = {
    assets::font, 35, sf::Color::White, sf::Color::Black, 2
};
/// Room code text settings.
static const ui::TextSettings k_CodeFont = {
    assets::font, 80, sf::Color::Yellow, sf::Color::Black, 4
};

/// Creates a full-screen container element.
static ui::Element* makeContainer() {
    auto* el = new ui::Element();
    el->bounds = { 0, 0, 1ps, 1ps };
    return el;
}

/// Applies text settings to a label.
static void applySettings(ui::Text* text, const ui::TextSettings& settings) {
    if (!text) return;
    text->setSize(settings.size);
    text->setColor(settings.fill);
    text->setOutline(settings.outline, settings.thickness);
}

/// Constructs a game start menu.
GameStartMenu::GameStartMenu() {
    bounds = { 0, 0, 1ps, 1ps };

    // Initialize main containers
    _mainSwitcher = new ui::Pages();
    _mainSwitcher->bounds = { 0, 0, 1ps, 1ps };
    add(_mainSwitcher);

    _setupContainer = new ui::Element();
    _setupContainer->bounds = { 0, 0, 1ps, 1ps };
    _mainSwitcher->add(_setupContainer);

     buildSidebar();
    _setupContainer->add(_sidebarBg);

    buildNavigation();
    _setupContainer->add(_navArea);

    // Initialize content pages container
    _contentPages = new ui::Pages();
    _contentPages->bounds = { 0.25ps, 0, 0.75ps, 0.85ps };
    _setupContainer->add(_contentPages);

    // Create pages and lobby
    _pageWaitingLobby = new LobbyMenu();
    _mainSwitcher->add(_pageWaitingLobby);

    _pageMode = createModePage();
    _pageDiff = createDiffPage();
    _pageMap = createMapPage();
    _pageLobby = createLobbyPage();

    _contentPages->add(_pageMode);
    _contentPages->add(_pageDiff);
    _contentPages->add(_pageMap);
    _contentPages->add(_pageLobby);

    // Default settings
    _selectedMode = Mode_None;
    _selectedDiff = Diff_None;
    _selectedMap = Map_None;
    _currentStep = STEP_MODE;
    _maxPlayers = 2;

    // Bind lobby callbacks
    _pageWaitingLobby->bindStart([this]() { if (_onStartGame) _onStartGame(); });
    _pageWaitingLobby->bindLeave([this]() {
        if (_selectedMode == Mode_Host && _currentStep == STEP_WAITING) {
            _currentStep = STEP_LOBBY;
            updateUI();
        }
        else {
            if (_onBack) _onBack();
        }
        });

    // register localization refresh listener and initialize text
    assets::lang::refresh_listeners.push_back([this]() { refreshAllText(); });
    refreshAllText();

    updateUI();
    generateGameCode();

}

/// Generates a new room code.
void GameStartMenu::generateGameCode() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(100000, 999999);
    _gameCode = std::to_string(dist(rng));

    if (_pageWaitingLobby) {
        _pageWaitingLobby->setRoomCode(_gameCode);
    }
}

/// Builds sidebar UI elements.
void GameStartMenu::buildSidebar() {
    _sidebarBg = new ui::Solid();
    _sidebarBg->color = sf::Color(15, 15, 25, 255);
    _sidebarBg->bounds = { 0, 0, 0.25ps, 1ps };

    auto addLabel = [this](ui::Dim yPos) -> ui::Text* {
        auto* lbl = ui::Text::raw(k_SidebarFont, "");
        lbl->bounds = { 0.1as, yPos, 0.8as, 0 };
        lbl->align = ui::Text::Center;
        lbl->pos = ui::Text::Static;
        _sidebarBg->add(lbl);
        return lbl;
    };

    /// Static label order.
    _lblMode = addLabel(0.25ps);
    _lblDiff = addLabel(0.35ps);
    _lblMap = addLabel(0.45ps);
    _lblLobby = addLabel(0.55ps);

    // set localization paths immediately so labels are visible at creation
    if (_lblMode) _lblMode->setPath("start.step1");
    if (_lblDiff) _lblDiff->setPath("start.step2");
    if (_lblMap) _lblMap->setPath("start.step3");
    if (_lblLobby) _lblLobby->setPath("start.step4");

    /// Back button.
    _backBtn = new menuui::Button();
    _backBtn->setSize({ 180px, 80px });
    _backBtn->position() = { 0.5as, 1as - 25px };
    _backBtn->setLabel()->setPath("menu.back");
    _backBtn->setCall([this]() {
        if (_onBack) _onBack();
    }, nullptr, menuui::Button::Click);
    _sidebarBg->add(_backBtn);
}

/// Builds navigation controls.
void GameStartMenu::buildNavigation() {
    _navArea = makeContainer();
    _navArea->bounds = { 0.25ps, 0.85ps, 0.75ps, 0.15ps };

    /// Previous button.
    _prevBtn = new menuui::Button();
    _prevBtn->setSize({ 180px, 80px });
    _prevBtn->position() = { 0.2as, 0.5as };
    _prevBtn->setLabel()->setPath("menu.prev");
    _prevBtn->setCall([this]() { prevStep(); }, nullptr, menuui::Button::Click);
    _navArea->add(_prevBtn);

    /// Next/start button.
    _nextBtn = new menuui::Button();
    _nextBtn->setSize({ 180px, 80px });
    _nextBtn->position() = { 0.8as, 0.5as };
    _nextBtn->setLabel()->setPath("menu.next");
    _nextBtn->setCall([this]() { nextStep(); }, nullptr, menuui::Button::Click);
    _navArea->add(_nextBtn);
}

/// Creates game mode selection page.
ui::Element* GameStartMenu::createModePage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "");
    title->setPath("menu.select_mode");
    title->bounds = { 0, 100px, 1ps, 0 };
    title->align = ui::Text::Center;
    title->pos = ui::Text::Static;
    page->add(title);
    _pageModeTitle = title;

    auto makeBtn = [this, page](ui::Dim xOff, const std::string& txt, GameMode val) {
        auto* btn = new menuui::Button();
        btn->setSize({ 320px, 200px });
        btn->position() = { 0.5as + xOff, 0.5as };
        btn->setLabel()->setRaw(txt);
        btn->setCall([this, btn, val]() { selectMode(val, btn); }, nullptr, menuui::Button::Click);
        page->add(btn);
        _modeButtons.push_back(btn);
    };

    // set labels via localization
    makeBtn(-250px, "", Mode_Single);
    _modeButtons.back()->setLabel()->setPath("menu.singleplayer");
    makeBtn(250px, "", Mode_Host);
    _modeButtons.back()->setLabel()->setPath("menu.multiplayer_host");

    return page;
}

/// Creates difficulty selection page.
ui::Element* GameStartMenu::createDiffPage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "");
    title->setPath("menu.select_difficulty");
    title->bounds = { 0, 100px, 1ps, 0 };
    title->align = ui::Text::Center;
    title->pos = ui::Text::Static;
    page->add(title);
    _pageDiffTitle = title;

    auto makeBtn = [this, page](ui::Dim xOff, const std::string& txt, Difficulty val) {
        auto* btn = new menuui::Button();
        btn->setSize({ 240px, 160px });
        btn->position() = { 0.5as + xOff, 0.5as };
        btn->setLabel()->setRaw(txt); 
        btn->setCall([this, btn, val]() { selectDifficulty(val, btn); }, nullptr, menuui::Button::Click);
        page->add(btn);
        _diffButtons.push_back(btn);
    };

    makeBtn(-300px, "", Diff_Easy);
    _diffButtons.back()->setLabel()->setPath("start.easy");
    makeBtn(0px, "", Diff_Medium);
    _diffButtons.back()->setLabel()->setPath("start.medium");
    makeBtn(300px, "", Diff_Hard);
    _diffButtons.back()->setLabel()->setPath("start.hard");

    return page;
}

/// Creates map selection page.
ui::Element* GameStartMenu::createMapPage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "");
    title->setPath("menu.select_map");
    title->bounds = { 0, 80px, 1ps, 0 };
    title->align = ui::Text::Center;
    title->pos = ui::Text::Static;
    page->add(title);
    _pageMapTitle = title;

    auto makeBtn = [this, page](ui::Dim xOff, const std::string& txt, MapID val) {
        auto* btn = new menuui::Button();
        btn->setSize({ 260px, 360px });
        btn->position() = { 0.5as + xOff, 0.5as };

        // map preview
        {
            sf::Vector2u size = assets::map_example.getSize();
            unsigned texW = size.x;
            unsigned texH = size.y;
            int cropSize = texW < texH ? texW : texH;
            int left = static_cast<int>((texW - cropSize) / 2);
            int top = static_cast<int>((texH - cropSize) / 2);

            sf::IntRect texRect({ left, top }, { cropSize, cropSize });

            auto* img = new ui::Image(&assets::map_example, texRect);
            img->bounds = { 0.1ps, 0.1ps, 0.8ps, 0.8ps };
            img->tint = sf::Color::White;
            btn->add(img);
        }

        btn->setLabel()->setRaw(txt); // temporary, replaced below per-button
        btn->setCall([this, btn, val]() { selectMap(val, btn); }, nullptr, menuui::Button::Click);

        page->add(btn);
        _mapButtons.push_back(btn);
    };

    makeBtn(-300px, "", Map_1);
    _mapButtons.back()->setLabel()->setPath("start.map1");
    makeBtn(0px, "", Map_2);
    _mapButtons.back()->setLabel()->setPath("start.map2");
    makeBtn(300px, "", Map_3);
    _mapButtons.back()->setLabel()->setPath("start.map3");

    return page;
}

/// Creates multiplayer lobby page.
ui::Element* GameStartMenu::createLobbyPage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "");
    title->setPath("menu.lobby_settings");
    title->bounds = { 0, 80px, 1ps, 0 };
    title->align = ui::Text::Center;
    title->pos = ui::Text::Static;
    page->add(title);
    _pageLobbyTitle = title;

    /// Max players section.
    _lblMaxPlayers = ui::Text::raw(k_SidebarFont, "");
    _lblMaxPlayers->setPath("menu.max_players");
    _lblMaxPlayers->bounds = { 0, 200px, 1ps, 0 };
    _lblMaxPlayers->align = ui::Text::Center;
    _lblMaxPlayers->pos = ui::Text::Static;
    page->add(_lblMaxPlayers);

    auto makeCountBtn = [this, page](ui::Dim xOff, int count) {
        auto* btn = new menuui::Button();
        btn->setSize({ 100px, 100px });
        btn->position() = { 0.5as + xOff, 280px };
        // use localization for the player count label
        btn->setLabel()->setPath("menu.player_count");
        btn->setLabel()->param("value", std::to_string(count));
        btn->setCall([this, btn, count]() { selectMaxPlayers(count, btn); }, nullptr, menuui::Button::Click);
        page->add(btn);
        _playerCountButtons.push_back(btn);
        if (count == 2) btn->select();
    };

    makeCountBtn(-120px, 2);
    makeCountBtn(0px, 3);
    makeCountBtn(120px, 4);

    /// Room code section.
    generateGameCode();

    _lblRoomCodeDesc = ui::Text::raw(k_SidebarFont, "");
    _lblRoomCodeDesc->setPath("menu.room_code_desc");
    _lblRoomCodeDesc->bounds = { 0, 450px, 1ps, 0 };
    _lblRoomCodeDesc->align = ui::Text::Center;
    _lblRoomCodeDesc->pos = ui::Text::Static;
    page->add(_lblRoomCodeDesc);

    _lblRoomCode = ui::Text::raw(k_CodeFont, "");
    _lblRoomCode->setPath("lobby.room_id");
    _lblRoomCode->bounds = { 0, 520px, 1ps, 0 };
    _lblRoomCode->align = ui::Text::Center;
    _lblRoomCode->pos = ui::Text::Static;
    _lblRoomCode->param("id", _gameCode);
    _lblRoomCode->hook([=]() mutable { _lblRoomCode->param("id", _gameCode); });
    page->add(_lblRoomCode);

    return page;
}

/// Selects game mode.
void GameStartMenu::selectMode(GameMode mode, menuui::Button* btn) {
    for (auto* b : _modeButtons) if (b != btn) b->deselect();
    bool modeChanged = (_selectedMode != mode);
    _selectedMode = mode;
    btn->select();
    if (modeChanged) {
        generateGameCode();
        updateUI();
    }
}

/// Selects max player count.
void GameStartMenu::selectMaxPlayers(int count, menuui::Button* btn) {
    for (auto* b : _playerCountButtons) if (b != btn) b->deselect();
    _maxPlayers = count;
    btn->select();
}

/// Selects difficulty.
void GameStartMenu::selectDifficulty(Difficulty diff, menuui::Button* btn) {
    for (auto* b : _diffButtons) if (b != btn) b->deselect();
    _selectedDiff = diff;
    btn->select();
    updateUI();
}

/// Selects map.
void GameStartMenu::selectMap(MapID map, menuui::Button* btn) {
    for (auto* b : _mapButtons) if (b != btn) b->deselect();
    _selectedMap = map;
    btn->select();
    updateUI();
}

/// Checks if current step can proceed.
bool GameStartMenu::canProceed() const {
    switch (_currentStep) {
    case STEP_MODE:  return _selectedMode != Mode_None;
    case STEP_DIFF:  return _selectedDiff != Diff_None;
    case STEP_MAP:   return _selectedMap != Map_None;
    case STEP_LOBBY: return true;
    case STEP_WAITING: return true;
    default: return false;
    }
}

/// Advances to next step.
void GameStartMenu::nextStep() {
    if (!canProceed()) return;

    // Transition logic from MAP step
    if (_currentStep == STEP_MAP) {
        if (_selectedMode == Mode_Single) {
            // If Singleplayer -> start the game
            if (_onStartGame) _onStartGame();
            return;
        }
        else {
            // If Host -> go to Lobby settings
            _currentStep = STEP_LOBBY;
            updateUI();
            return;
        }
    }

    // Transition from LOBBY settings to WAITING lobby (host only)
    if (_currentStep == STEP_LOBBY) {
        _currentStep = STEP_WAITING;

        _pageWaitingLobby->setRoomCode(_gameCode);
        _pageWaitingLobby->setAsHost(true);
        _pageWaitingLobby->updateList({ { assets::lang::locale.req(localization::Path("lobby.host_you")).get({}), sf::Color::Cyan, true, true } });

        updateUI();
        return;
    }

    // If in WAITING and Start Game is clicked
    if (_currentStep == STEP_WAITING) {
        if (_onStartGame) _onStartGame();
        return;
    }

    // Standard step advance for MODE and DIFF
    _currentStep++;
    updateUI();
}

/// Goes to previous step.
void GameStartMenu::prevStep() {
    if (_currentStep > STEP_MODE) {
        _currentStep--;
        updateUI();
    }
}

/// Updates menu UI.
void GameStartMenu::updateUI() {
    if (_currentStep == STEP_WAITING) {
        // Show only the waiting lobby: Sidebar and NavArea are hidden
        _mainSwitcher->show(_pageWaitingLobby);
    }
    else {
        _mainSwitcher->show(_setupContainer);

        // Show subpage inside setup
        ui::Element* subPage = nullptr;
        switch (_currentStep) {
        case STEP_MODE:  subPage = _pageMode;  break;
        case STEP_DIFF:  subPage = _pageDiff;  break;
        case STEP_MAP:   subPage = _pageMap;   break;
        case STEP_LOBBY: subPage = _pageLobby; break;
        }
        _contentPages->show(subPage);
    }

    updateSidebarLabels();
    updateNavigationButtons();
}

/// Updates sidebar labels.
void GameStartMenu::updateSidebarLabels() {
    if (!_lblMode || !_lblDiff || !_lblMap || !_lblLobby) return;

    // ensure sidebar labels are active/inactive and styled, text is provided via localization paths
    _lblMode->activate();
    applySettings(_lblMode, (_currentStep == STEP_MODE) ? k_SidebarFontActive : k_SidebarFont);
    
    _lblDiff->activate();
    applySettings(_lblDiff, (_currentStep == STEP_DIFF) ? k_SidebarFontActive : k_SidebarFont);
    

    _lblMap->activate();
    applySettings(_lblMap, (_currentStep == STEP_MAP) ? k_SidebarFontActive : k_SidebarFont);


    if (_selectedMode == Mode_Host) {
        _lblLobby->activate();
        bool isLobbyActive = (_currentStep == STEP_LOBBY || _currentStep == STEP_WAITING);
        applySettings(_lblLobby, isLobbyActive ? k_SidebarFontActive : k_SidebarFont);
    }
    else {
        // In singleplayer the lobby point does not exist
        _lblLobby->deactivate();
    }
}

/// Updates navigation buttons.
void GameStartMenu::updateNavigationButtons() {
    if (!_navArea || !_prevBtn || !_nextBtn) return;

    if (_currentStep == STEP_WAITING) {
        _navArea->deactivate();
        return;
    }

    _navArea->activate();
    _prevBtn->display = (_currentStep > STEP_MODE);

    // Check if current step is the last for the selected mode
    bool isLastStepForMode = false;
    if (_selectedMode == Mode_Single) {
        isLastStepForMode = (_currentStep == STEP_MAP);
    }
    else {
        // For host the last step before waiting is STEP_LOBBY
        isLastStepForMode = (_currentStep == STEP_LOBBY);
    }

    if (isLastStepForMode)
        _nextBtn->setLabel()->setPath("menu.start_game");
    else
        _nextBtn->setLabel()->setPath("menu.next");
   
    // Activate NEXT only if selection is made
    if (canProceed()) {
        _nextBtn->setLabel()->setColor(sf::Color::White);
        _nextBtn->setCall([this]() { nextStep(); }, nullptr, menuui::Button::Click);
    }
    else {
        _nextBtn->setLabel()->setColor(sf::Color(100, 100, 100));
        _nextBtn->setCall(nullptr, nullptr, menuui::Button::Click);
    }
}


/// Binds back callback.
void GameStartMenu::bindBack(Action action) {
    _onBack = action;
}

/// Binds start callback.
void GameStartMenu::bindStart(Action action) {
    _onStartGame = action;
}

/// Draws the menu.
void GameStartMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}

/// Enters the menu as a joiner with the given game code.
void GameStartMenu::enterAsJoiner(const std::string& code) {
    _selectedMode = Mode_None;
    _gameCode = code;
    _currentStep = STEP_WAITING;
    updateNavigationButtons();
    updateSidebarLabels();
    _pageWaitingLobby->setRoomCode(_gameCode);
    _pageWaitingLobby->setAsHost(false);
    _pageWaitingLobby->updateList({ { assets::lang::locale.req(localization::Path("lobby.connecting")).get({}), sf::Color::White, false, false} });

    updateUI();
}

/// Refreshes all text elements for localization.
void GameStartMenu::refreshAllText() {
    _lblMode->setPath("start.step1"); 
    _lblDiff->setPath("start.step2");
    _lblMap->setPath("start.step3");
    _lblLobby->setPath("start.step4");

    if (_pageModeTitle) _pageModeTitle->setPath("menu.select_mode");
    if (_pageDiffTitle) _pageDiffTitle->setPath("menu.select_difficulty");
    if (_pageMapTitle) _pageMapTitle->setPath("menu.select_map");
    if (_pageLobbyTitle) _pageLobbyTitle->setPath("menu.lobby_settings");

    if (_lblMaxPlayers) _lblMaxPlayers->setPath("menu.max_players");
    if (_lblRoomCodeDesc) _lblRoomCodeDesc->setPath("menu.room_code_desc");
    if (_lblRoomCode) {
        _lblRoomCode->setPath("lobby.room_id");
        _lblRoomCode->param("id", _gameCode);
    }

    // Update mode buttons labels
    for (size_t i = 0; i < _modeButtons.size(); ++i) {
        if (!_modeButtons[i]) continue;
        ui::Text* lbl = _modeButtons[i]->setLabel();
        if (!lbl) continue;
        if (i == 0) lbl->setPath("menu.singleplayer");
        else if (i == 1) lbl->setPath("menu.multiplayer_host");
    }

    // Update difficulty buttons labels
    for (size_t i = 0; i < _diffButtons.size(); ++i) {
        if (!_diffButtons[i]) continue;
        ui::Text* lbl = _diffButtons[i]->setLabel();
        if (!lbl) continue;
        if (i == 0) lbl->setPath("start.easy");
        else if (i == 1) lbl->setPath("start.medium");
        else if (i == 2) lbl->setPath("start.hard");
    }

    // Update map buttons labels
    for (size_t i = 0; i < _mapButtons.size(); ++i) {
        if (!_mapButtons[i]) continue;
        ui::Text* lbl = _mapButtons[i]->setLabel();
        if (!lbl) continue;
        if (i == 0) lbl->setPath("start.map1");
        else if (i == 1) lbl->setPath("start.map2");
        else if (i == 2) lbl->setPath("start.map3");
    }

    // Update player count buttons
    for (size_t i = 0; i < _playerCountButtons.size(); ++i) {
        if (!_playerCountButtons[i]) continue;
        ui::Text* lbl = _playerCountButtons[i]->setLabel();
        if (!lbl) continue;
        lbl->setPath("menu.player_count");
        // set parameter value to the button's numeric label
        int val = 2 + (int)i;
        lbl->param("value", std::to_string(val));
    }

    // navigation and sidebar buttons
    _backBtn->setLabel()->setPath("menu.back");
    _prevBtn->setLabel()->setPath("menu.prev");
    _nextBtn->setLabel()->setPath("menu.next");

    updateNavigationButtons();
}

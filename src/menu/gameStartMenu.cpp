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
GameStartMenu::GameStartMenu(Net* net) : _net(net) {
    bounds = { 0, 0, 1ps, 1ps };

    _hasSelectedMode = false;
    _hasSelectedDiff = false;
    _currentData.selectedMapPath = ""; 
    _currentStep = STEP_MODE;

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

    _contentPages = new ui::Pages();
    _contentPages->bounds = { 0.25ps, 0, 0.75ps, 0.85ps };
    _setupContainer->add(_contentPages);

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

    _pageWaitingLobby->bindStart([this]() { if (_onStartGame) _onStartGame(); });
    _pageWaitingLobby->bindLeave([this]() {
        if (_currentData.isMultiplayer && _currentStep == STEP_WAITING) {
            _currentStep = STEP_LOBBY;
            updateUI();
        } else if (_onBack) {
            _onBack();
        }
    });

    assets::lang::refresh_listeners.push_back([this]() { refreshAllText(); });
    
    generateGameCode();
    refreshAllText();
    updateUI();
}

/// Generates a new room code.
void GameStartMenu::generateGameCode() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(100000, 999999);
    _currentData.roomCode = std::to_string(dist(rng));

    if (_pageWaitingLobby) {
        _pageWaitingLobby->setRoomCode(_currentData.roomCode);
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
    
    _lblLobby->deactivate();

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
    _lblRoomCode->param("id", _currentData.roomCode);
    _lblRoomCode->hook([=]() mutable { _lblRoomCode->param("id",  _currentData.roomCode); });
    page->add(_lblRoomCode);

    return page;
}

/// Selects game mode.
void GameStartMenu::selectMode(GameMode mode, menuui::Button* btn) {
    for (auto* b : _modeButtons) b->deselect();
    btn->select();

    // Map UI Selection to GameData
    _currentData.isMultiplayer = (mode == Mode_Host);
   
     _hasSelectedMode = true; 

    if (_currentData.isMultiplayer) {
        generateGameCode();
    }
    updateUI();
}

/// Selects max player count.
void GameStartMenu::selectMaxPlayers(int count, menuui::Button* btn) {
    for (auto* b : _playerCountButtons) if (b != btn) b->deselect();
     _currentData.maxPlayers = static_cast<unsigned int>(count);
    btn->select();
}

/// Selects difficulty.
void GameStartMenu::selectDifficulty(Difficulty diff, menuui::Button* btn) {
    for (auto* b : _diffButtons) if (b != btn) b->deselect();
    _currentData.difficulty = static_cast<GameData::Difficulty>(diff);
   _hasSelectedDiff = true;
    btn->select();
    updateUI();
}


/// Checks if current step can proceed.
bool GameStartMenu::canProceed() const {
    switch (_currentStep) {
        case STEP_MODE: return _hasSelectedMode;
        case STEP_DIFF: return _hasSelectedDiff;
        case STEP_MAP:  return !_currentData.selectedMapPath.empty();
        default:        return true;
    }
}


/// Advances to next step.
void GameStartMenu::nextStep() {
    if (!canProceed()) return;

    if (_currentStep == STEP_MAP) {
        if (!_currentData.isMultiplayer) {
            if (_onStartGame) _onStartGame();
            return;
        } else {
            _currentStep = STEP_LOBBY;
            updateUI();
            return;
        }
    }

    if (_currentStep == STEP_LOBBY) {
        _currentStep = STEP_WAITING;

		_net->host(_currentData.maxPlayers, _currentData.roomCode);
        _pageWaitingLobby->setGameDetails(_currentData);
        _pageWaitingLobby->setAsHost(true);
        _pageWaitingLobby->updateList({ { assets::lang::locale.req(localization::Path("lobby.host_you")).get({}), sf::Color::Cyan, true, true } });
        
        updateUI();
        return;
    }

    if (_currentStep == STEP_WAITING) {
        if (_onStartGame) _onStartGame();
        return;
    }

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
        _mainSwitcher->show(_pageWaitingLobby);
    } else {
        _mainSwitcher->show(_setupContainer);
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


    if (_hasSelectedMode && _currentData.isMultiplayer) {
        _lblLobby->activate();
        bool isLobbyActive = (_currentStep == STEP_LOBBY || _currentStep == STEP_WAITING);
        applySettings(_lblLobby, isLobbyActive ? k_SidebarFontActive : k_SidebarFont);
    } else {
        _lblLobby->deactivate();
    }
}

/// Updates navigation buttons.
void GameStartMenu::updateNavigationButtons() {
    if (!_navArea || !_nextBtn) return;

    if (_currentStep == STEP_WAITING) {
        _navArea->deactivate();
        return;
    }

    _navArea->activate();
    _prevBtn->display = (_currentStep > STEP_MODE);

    bool isLastStep = (!_currentData.isMultiplayer && _currentStep == STEP_MAP) || 
                      (_currentData.isMultiplayer && _currentStep == STEP_LOBBY);

    _nextBtn->setLabel()->setPath(isLastStep ? "menu.start_game" : "menu.next");

     if (canProceed()) {
        _nextBtn->setLabel()->setColor(sf::Color::White);
    } else {
        _nextBtn->setLabel()->setColor(sf::Color(100, 100, 100));
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
    _currentData.isMultiplayer = true;
    _currentData.roomCode = code;
    
    _currentStep = STEP_WAITING;

    _pageWaitingLobby->setRoomCode(_currentData.roomCode);
    _pageWaitingLobby->setAsHost(false);     
    
    PlayerData connectingMsg;
    connectingMsg.name = assets::lang::locale.req(localization::Path("lobby.connecting")).get({});
    connectingMsg.color = sf::Color::White;
    connectingMsg.isHost = false;
    connectingMsg.isReady = false;

    _pageWaitingLobby->updateList({ connectingMsg });

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
        _lblRoomCode->param("id", _currentData.roomCode);
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

    updateMapGrid();

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
    updateSidebarLabels();
}

/// Scans the maps folder for available maps.
void GameStartMenu::scanMaps() {
    _availableMaps.clear();
    _scanDiagnostic = ""; 
    
    namespace fs = std::filesystem;
    std::string foundPath = std::string(ASSET_PATH) + "/maps/";
    if (foundPath.empty()) {
        _scanDiagnostic = "assets/maps/ not found.";
        return;
    }

    for (const auto& entry : fs::directory_iterator(foundPath)) {
        // Check for JSON
        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".json") {
            MapInfo info;
            info.filePath = entry.path().string();
            info.id = entry.path().stem().string();

            // Generate Display Name
            std::string name = info.id;
            std::replace(name.begin(), name.end(), '_', ' ');
            if (!name.empty()) name[0] = std::toupper(name[0]);
			info.displayName = name; 
            // TODO: should probably be read from JSON in future

            auto pngPath = entry.path().parent_path() / (info.id + ".png");
            info.previewPath = pngPath.string();
            info.texture = std::make_shared<sf::Texture>();
          
 
            if (info.texture->loadFromFile(info.previewPath)) {
                //printf("[SUCCESS] Loaded map preview: %s\n", info.previewPath.c_str());
            } else {
                printf("[FAILED]  Could not load image: %s\n", info.previewPath.c_str());
                info.texture = nullptr; 
            }

            _availableMaps.push_back(info);
        }
    }
}

/// Creates map selection page.
ui::Element* GameStartMenu::createMapPage() {
    auto* page = makeContainer();

    _pageMapTitle = ui::Text::raw(k_HeaderFont, "");
    _pageMapTitle->setPath("menu.select_map");
    _pageMapTitle->bounds = { 0, 80px, 1ps, 0 };
    _pageMapTitle->align = ui::Text::Center;
    page->add(_pageMapTitle);

    _mapGrid = new ui::Element();
    _mapGrid->bounds = { 0, 0, 1ps, 1ps };
    page->add(_mapGrid);

    scanMaps();

    _mapPrevBtn = new menuui::Button();
    _mapPrevBtn->setSize({ 60px, 100px });
    _mapPrevBtn->position() = { 50px, 0.5as };
    _mapPrevBtn->setLabel()->setRaw("<");
    _mapPrevBtn->setCall([this]() { changeMapPage(-1); }, nullptr, menuui::Button::Click);
    page->add(_mapPrevBtn);

    _mapNextBtn = new menuui::Button();
    _mapNextBtn->setSize({ 60px, 100px });
    _mapNextBtn->position() = { 1ps - 100px, 0.5as };
    _mapNextBtn->setLabel()->setRaw(">");
    _mapNextBtn->setCall([this]() { changeMapPage(1); }, nullptr, menuui::Button::Click);
    page->add(_mapNextBtn);

    updateMapGrid();
    return page;
}

/// Updates the map grid display.
void GameStartMenu::updateMapGrid() {
    _mapGrid->clear();
    _mapButtons.clear();

    if (_availableMaps.empty()) {
        auto* lbl = ui::Text::raw(k_SidebarFont, _scanDiagnostic.empty() ? "No maps found." : _scanDiagnostic);
        lbl->bounds = { 20px, 0.4ps, 1ps - 40px, 0 };
        lbl->align = ui::Text::Center;
        _mapGrid->add(lbl);
        return;
    }

    int mapsPerPage = 3;
    float startX = -300.0f;

    for (int i = 0; i < mapsPerPage; ++i) {
        int idx = _mapPageOffset + i;
        if (idx >= (int)_availableMaps.size()) break;

        const auto& map = _availableMaps[idx];
        auto* btn = new menuui::Button();
        btn->setSize({ 260px, 360px });
        btn->position() = { 0.5as + (startX + (i * 300.0f)), 0.5as };
        
        if (map.texture) {
            sf::Vector2u size = map.texture->getSize();
            sf::IntRect fullRect({0, 0}, { (int)size.x, (int)size.y });

            auto* img = new ui::Image(map.texture.get(), fullRect);
            
            img->bounds = { 0.1ps, 0.1ps, 0.8ps, 0.6ps };
            img->tint = sf::Color::White;
            btn->add(img);
        } else {
            auto* placeholder = new ui::Solid();
            placeholder->color = sf::Color(50, 50, 50);
            placeholder->bounds = { 0.1ps, 0.1ps, 0.8ps, 0.6ps };
            btn->add(placeholder);
        }

        btn->setLabel()->setRaw(map.displayName);
        btn->setLabel()->bounds = {0, 0.7ps, 1ps, 0.3ps};
        
        btn->setCall([this, btn, map]() {
            for (auto* b : _mapButtons) b->deselect();
            btn->select();
            _currentData.selectedMapPath = map.filePath;
            _currentData.selectedMapName = map.displayName;
            updateUI();
        }, nullptr, menuui::Button::Click);

        if (_currentData.selectedMapPath == map.filePath) btn->select();
        _mapGrid->add(btn);
        _mapButtons.push_back(btn);
    }

    _mapPrevBtn->display = (_mapPageOffset > 0);
    _mapNextBtn->display = (_mapPageOffset + mapsPerPage < (int)_availableMaps.size());
}


/// Changes the map page by delta.
void GameStartMenu::changeMapPage(int delta) {
    int mapsPerPage = 3;
    int newOffset = _mapPageOffset + (delta * mapsPerPage);

    if (newOffset < 0) return;

    if (newOffset >= (int)_availableMaps.size()) return;

    _mapPageOffset = newOffset;
    updateMapGrid();
}
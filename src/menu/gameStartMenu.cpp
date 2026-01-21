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

static void setButtonEnabled(menuui::Button* btn, bool enabled, const std::function<void()>& call) {
    if (!btn) return;

    if (enabled) {
        btn->setCall(call, nullptr, menuui::Button::Click);
        btn->setLabel()->setColor(sf::Color::White);
    } else {
        btn->setCall(nullptr, nullptr, menuui::Button::Click);
        btn->setLabel()->setColor(sf::Color(100, 100, 100));
    }
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

    _isHost = false;

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

    _pageWaitingLobby->bindStart([this]() { 
        if (_onStartGame) _onStartGame(); 
        nextStep();
    });
    
    _pageWaitingLobby->bindLeave([this]() {
        if (_currentData.isMultiplayer && _currentStep == STEP_WAITING) {
            if (_net) {
                sf::Packet pkt;
                if (_isHost) {
                    // HOST: Tell everyone the party is over
                    pkt << (int)Pkt_LobbyEnded;
                    _net->send(pkt);
                    printf("[Host] Broadcasted LobbyEnded packet.\n");
                } 
                else {
                    // CLIENT: Tell Host I am leaving (so he updates UI)
                    pkt << (int)Pkt_Leave;
                    _net->send(pkt);
                }
            }

            if (_net) _net->leaveLobby();
            
            _currentData.isMultiplayer = false; 
            return;
        }
        if (_onBack) _onBack();
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

void GameStartMenu::setControlsLocked(bool locked) {
    _controlsLocked = locked;

    setButtonEnabled(_backBtn, !locked, [this]() {
        if (_onBack) _onBack();
    });

    setButtonEnabled(_prevBtn, !locked, [this]() { prevStep(); });

    // Next button jest sterowany dodatkowo przez canProceed() / etapy,
    // wiêc po locku wymuœ updateNavigationButtons (ustawi call/kolor wg stanu).
    updateNavigationButtons();
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
    
    if (mode == Mode_Host) {
        // Check if logged in before allowing Host selection
        if (!_net->isLoggedIn()) {
            _alert->show("You must be logged in\nto play Multiplayer!");
            return; // Stop here, do not select the button
        }
        // Clean state before becoming host
        resetLobbyState(); 
    }
    
    for (auto* b : _modeButtons) b->deselect();
    btn->select();

    // Map UI Selection to GameData
    _currentData.isMultiplayer = (mode == Mode_Host);
    _isHost = (mode == Mode_Host);

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
    if (_controlsLocked) return;
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
        // NIE przechodŸ do WAITING zanim EOS potwierdzi utworzenie lobby.
        // Najpierw podpinamy callbacki jak w join flow.
        if (_net) {
            _net->clearHandlers();
        }

        // Lock UI zanim ruszy EOS
        setControlsLocked(true);

        if (_net) {
            _net->OnLobbySuccess.add([this]() {
                // Lobby istnieje w backendzie -> teraz bezpiecznie wejdŸ do ekranu lobby
                setControlsLocked(false);

                _currentStep = STEP_WAITING;

                _localPlayerName = _net->getLocalDisplayName(); 

                _pageWaitingLobby->setGameDetails(_currentData);
                _pageWaitingLobby->setAsHost(true);
                _pageWaitingLobby->setRoomCode(_currentData.roomCode);
                //_pageWaitingLobby->setInteractable(true);

                addSelfToUI();
                bindNetworkHandlers();
                updateUI();
            });

            _net->OnJoinFailed.add([this](const std::string&) {
                // Hosting/Join nie doszed³ do skutku -> odblokuj kontrolki,
                // zostaw usera na konfiguracji.
                setControlsLocked(false);
                updateUI();
            });

            _net->host(_currentData.maxPlayers, _currentData.roomCode);
        }

        return;
    }

    if (_currentStep == STEP_WAITING) {
        if (_currentData.isMultiplayer && _isHost && _net) {
            sf::Packet pkt;
            pkt << (int)Pkt_StartGame;
            _net->send(pkt);
            printf("[Host] Broadcasting StartGame packet...\n");
        }

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
    
	// this is AI SLOP GARBAGE GET RID OF IT PROBABLY
    if (_currentData.isMultiplayer && !_isHost && !_acknowledgedByHost) {
        GameStartMenu* mutableSelf = const_cast<GameStartMenu*>(this);
        
        if (mutableSelf->_heartbeatTimer.getElapsedTime().asSeconds() > 1.0f) {
            mutableSelf->_heartbeatTimer.restart();
            mutableSelf->sendHello();
            printf("[Client] Heartbeat: Sending Hello...\n");
        }
    }
}

/// Enters the menu as a joiner with the given game code.
void GameStartMenu::enterAsJoiner(const std::string& code) {
    // 1. Reset everything first
    resetLobbyState();

    // 2. Setup Client State
    _currentData.isMultiplayer = true;
    _currentData.roomCode = code;
    _localPlayerName = _net->getLocalDisplayName();     
    _currentStep = STEP_WAITING;
    
    // 3. Update UI
    updateUI(); 
    if (_pageWaitingLobby) {
        _pageWaitingLobby->setRoomCode(code);
    }
    _pageWaitingLobby->setAsHost(false);
    
    // 4. Initial "Connecting" visual
    PlayerData p;
    p.name = "Connecting...";
    p.color = sf::Color(150, 150, 150);
    _pageWaitingLobby->updateList({ p });

    // 5. Start Logic
    bindNetworkHandlers();
    sendHello();

    // Odblokuj kontrolki po pierwszym backendowym potwierdzeniu stanu lobby
    if (_net) {
        _net->OnPacketReceived.add([this](const std::string&, sf::Packet& pkt) {
            int type;
            sf::Packet copy = pkt;
            if (!(copy >> type)) return;

            if (type == Pkt_LobbyState) {
                setControlsLocked(false);
            }
        });
    }
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

void GameStartMenu::bindNetworkHandlers() {
    _net->clearHandlers();

    // HOST ONLY: Wait for lobby creation to finish
    _net->OnLobbySuccess.add([this]() {
        if (_isHost) { // Host
             addSelfToUI();
        } 
        // Client doesn't need this, they call sendHello() manually in enterAsJoiner
    });

    // Peer Connected
    _net->OnPlayerConnected.add([this](const std::string& id) {
        onPlayerJoined(id);
    });

    // Peer Disconnected
    _net->OnPlayerDisconnected.add([this](const std::string& id) {
        onPlayerLeft(id);
    });

    // Packets
    _net->OnPacketReceived.add([this](const std::string& id, sf::Packet& pkt) {
        onPacket(id, pkt);
    });
}

void GameStartMenu::onPlayerJoined(const std::string& id) {
    std::cout << "[Lobby] Player Connected: " << id << std::endl;

    if (_isHost) {
        // A client joined. Add placeholder and send them the map info.
        LobbyMember mem;
        mem.netId = id;
        mem.data.name = "Connecting..."; // Temp name until Hello packet
        mem.data.color = sf::Color::Green;
        mem.data.isHost = false;
        _connectedMembers.push_back(mem);
        
        // Immediately broadcast state so the new client receives map info and starts loading UI.
        broadcastLobbyState();
    } else {
        // IF CLIENT: Another peer joined. 
        // We do nothing specific here; we rely on the Host sending an updated LobbyState 
        // to update our list.
    }
}

void GameStartMenu::onPlayerLeft(const std::string& id) {
    // 1. Remove player from the internal vector
    auto it = std::remove_if(_connectedMembers.begin(), _connectedMembers.end(),
        [&](const LobbyMember& m) { return m.netId == id; });
    
    bool removed = (it != _connectedMembers.end());
    _connectedMembers.erase(it, _connectedMembers.end());

    // 2. Refresh UI and Network
    if (_currentData.isMultiplayer) {
        if (_isHost) {
            if (removed) {
                printf("[Lobby] Player %s left, updating list.\n", id.c_str());
            }
            // Host: Re-generates the UI list and sends new state to all clients
            broadcastLobbyState(); 
        } else {
            // Client: Do nothing. The Host will notice the leave, 
            // update their state, and send us a new Pkt_LobbyState.
        }
    }
}


void GameStartMenu::onPacket(const std::string& id, sf::Packet& pkt) {
    int type;
    if (!(pkt >> type)) return;

   
    if (type == Pkt_LobbyEnded) {
        if (!_isHost) {
            // 1. Show Popup
            if (_alert) _alert->show("The Host has ended the game.");
            
            // 2. Force Leave
            // This triggers OnLobbyLeft -> MenuSystem switches page.
            if (_net) _net->leaveLobby(); 
        }
        return;
    }
    // --- NEW: Handle Client Leaving (Host side) ---
    if (type == Pkt_Leave) {
        if (_isHost) {
            onPlayerLeft(id);
        }
        return;
    }

    // HOST LOGIC: Handle Hello from Clients
    if (_isHost && type == Pkt_Hello) {
        std::string name; 
        uint8_t r, g, b;
        if (pkt >> name >> r >> g >> b) {
            bool found = false;
            for (auto& m : _connectedMembers) {
                if (m.netId == id) {
                    m.data.name = name;
                    m.data.color = sf::Color(r,g,b);
                    m.data.isReady = true;
                    found = true;
                }
            }
            // Fallback if not found in list yet
            if (!found) {
                LobbyMember newMem;
                newMem.netId = id;
                newMem.data.name = name;
                newMem.data.color = sf::Color(r,g,b);
                newMem.data.isReady = true;
                _connectedMembers.push_back(newMem);
            }
            // Update Host UI and tell everyone the new list
            broadcastLobbyState(); 
        }
    }

    // CLIENT LOGIC: Handle State Update from Host
   else if (!_isHost && type == Pkt_LobbyState) {
    std::string mapName;
    std::string mapPath; 
    std::string roomCode; 
    int diff, maxP;
    uint32_t count;
    
   if (pkt >> roomCode >> mapName >> mapPath >> diff >> maxP >> count){
        std::string savedCode = _currentData.roomCode;
        // Update local data
        _currentData.roomCode = roomCode;
        _currentData.selectedMapName = mapName;
        _currentData.selectedMapPath = mapPath;
        _currentData.difficulty = (GameData::Difficulty)diff;
        _currentData.maxPlayers = (unsigned int)maxP;
        
        // This updates the "Map: ... Difficulty: ..." labels
        _pageWaitingLobby->setGameDetails(_currentData); 
        _pageWaitingLobby->setRoomCode(roomCode); 
        
        std::vector<PlayerData> uiList;
        bool foundMe = false;

        for(uint32_t i = 0; i < count; ++i) {
            PlayerData p;
            bool isHost;
            uint8_t r, g, b; // Use SFML explicit types

            if (pkt >> p.name >> isHost >> r >> g >> b) {
                p.isHost = isHost;
                p.color = sf::Color(r, g, b);
                p.isReady = true;
                uiList.push_back(p);
                
                // Check if the host has processed our Hello packet yet
                if (p.name == _localPlayerName) {
                    foundMe = true;
                }
            }
        }
        
        if (foundMe) {
            _acknowledgedByHost = true;
        }

        // Update the visual list (removes "Connecting...")
        _currentData.players = uiList; 
        _pageWaitingLobby->updateList(uiList);
        }
    }
    //START GAME LOGIC (Both)
    else if (type == Pkt_StartGame) {
        printf("[Client] Received StartGame packet. Switching context...\n");
        // This triggers the EXACT SAME callback as the Host's button click
        if (_onStartGame) _onStartGame(); 
    }
}


void GameStartMenu::sendHello() {
    sf::Packet pkt;
    pkt << (int)Pkt_Hello;
    // Send our local generated name so the host knows who we are
    pkt << _localPlayerName;
    pkt << (uint8_t)0 << (uint8_t)255 << (uint8_t)0; // Green color
    _net->send(pkt);
}




// Helper to add the local player to the UI list (Host only uses this)
void GameStartMenu::addSelfToUI() {
    _connectedMembers.clear();
    
    // In a real app, get this from AuthManager
    PlayerData hostMe;
    hostMe.name = _net->getLocalDisplayName() + " (Host)"; 
    hostMe.isHost = true;
    hostMe.color = sf::Color::Cyan;
    hostMe.isReady = true;
   
    std::vector<PlayerData> uiList;
    uiList.push_back(hostMe);
    _pageWaitingLobby->updateList(uiList);
}

void GameStartMenu::broadcastLobbyState() {
    std::vector<PlayerData> allPlayers;
    
    // Add Host (using local data)
    PlayerData host;
    host.name = _localPlayerName; 
    host.isHost = true;
    host.color = sf::Color::Cyan;
    allPlayers.push_back(host);

    // Add Clients from our tracked list
    for(auto& m : _connectedMembers) {
        allPlayers.push_back(m.data);
    }

    // Update local Host UI so the host sees the changes immediately
    _pageWaitingLobby->updateList(allPlayers);

    // Pack the packet
    sf::Packet pkt;
    pkt << (int)Pkt_LobbyState;

    pkt << _currentData.roomCode;

    pkt << _currentData.selectedMapName;
   
    if (_currentData.selectedMapPath.empty()) {
        pkt << "STATIC_MAP_ID"; 
    } else {
        pkt << _currentData.selectedMapPath;
    }

    pkt << (int)_currentData.difficulty;
    pkt << (int)_currentData.maxPlayers;
    pkt << (uint32_t)allPlayers.size();
    
    for(const auto& p : allPlayers) {
        // Use explicit casts to ensure cross-platform serialization matches
        pkt << p.name << p.isHost << (uint8_t)p.color.r << (uint8_t)p.color.g << (uint8_t)p.color.b;
    }
    
    _net->send(pkt);
}

void GameStartMenu::resetLobbyState() {
    _connectedMembers.clear();
    _isHost = false;
    _acknowledgedByHost = false;
    _currentData.isMultiplayer = false;
    _heartbeatTimer.restart();
    _net->clearHandlers(); // Critical: Stop receiving packets for the old session
}

void GameStartMenu::reset() {
    // 1. Reset Logic Steps
    _currentStep = STEP_MODE;
    _hasSelectedMode = false;
    _hasSelectedDiff = false;
    
    // 2. Reset Data
    _isHost = false;
    _acknowledgedByHost = false;
    _currentData.isMultiplayer = false;
    _currentData.roomCode = "";
    
    // 3. Clear Network Lists
    _connectedMembers.clear();
    _net->clearHandlers(); // Stop listening to packets
    
    // 4. Reset UI Elements
    if (_modeButtons.size() > 0) {
        for (auto* b : _modeButtons) b->deselect();
    }
    
    // 5. Apply Changes
    generateGameCode(); // New code for next time
    updateUI();         // This switches the view back to the Mode Selection page
}

void GameStartMenu::sendLeavePacket() {
    if (!_net) return;
    
    sf::Packet pkt;
    pkt << (int)Pkt_Leave;
    _net->send(pkt);
    
    printf("[Client] Sent Leave packet.\n");
}


const GameData& GameStartMenu::getGameData() {
    // 1. Save local name
    _currentData.localPlayerName = _localPlayerName; 

    // 2. Logic to populate player list based on role
    if (_currentData.isMultiplayer) {
        
        if (_isHost) {
            // HOST LOGIC: Rebuild list from Self + Connected Members
            _currentData.players.clear();

            // A. Add Host (Me)
            PlayerData hostMe;
            hostMe.name = _localPlayerName; //+ " (Host)"; 
            hostMe.color = sf::Color::Cyan; // Or whatever color you picked
            hostMe.isHost = true;
            _currentData.players.push_back(hostMe);

            // B. Add Clients
            for (const auto& mem : _connectedMembers) {
                _currentData.players.push_back(mem.data);
            }
        } 
        else {
            // CLIENT LOGIC: 
            // Do nothing here. We received the full list (including Host and other peers)
            // in onPacket -> Pkt_LobbyState, and we saved it to _currentData.players there.
             if (_currentData.players.empty()) {
                printf("[Warning] Client GameData has no players!\n");
            }
        }

    } else {
        // SINGLEPLAYER LOGIC
        _currentData.players.clear();
        PlayerData me;
        me.name = "Player";
        me.color = sf::Color::Red;
        _currentData.players.push_back(me);
    }

    return _currentData;
}
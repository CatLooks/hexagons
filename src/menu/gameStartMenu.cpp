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

    // 1. Najpierw zainicjalizuj g³ówne kontenery (szkielet)
    _mainSwitcher = new ui::Pages();
    _mainSwitcher->bounds = { 0, 0, 1ps, 1ps };
    add(_mainSwitcher);

    _setupContainer = new ui::Element();
    _setupContainer->bounds = { 0, 0, 1ps, 1ps };
    _mainSwitcher->add(_setupContainer);

    // 2. Zainicjalizuj paski (Sidebar i Nawigacja) - musz¹ istnieæ przed stronami!
    buildSidebar();
    // UWAGA: Wewn¹trz buildSidebar usuñ liniê 'add(_sidebarBg);' 
    // bo dodajemy go poni¿ej do w³aœciwego kontenera
    _setupContainer->add(_sidebarBg);

    buildNavigation();
    _setupContainer->add(_navArea);

    // 3. Zainicjalizuj kontener na strony
    _contentPages = new ui::Pages();
    _contentPages->bounds = { 0.25ps, 0, 0.75ps, 0.85ps };
    _setupContainer->add(_contentPages);

    // 4. Dopiero teraz twórz strony i Lobby (mog¹ one teraz bezpiecznie wywo³aæ updateUI)
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

    // 5. Ustawienia domyœlne
    _selectedMode = Mode_None;
    _selectedDiff = Diff_None;
    _selectedMap = Map_None;
    _currentStep = STEP_MODE;
    _maxPlayers = 2;

    // 6. Binduj callbacki Lobby
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

    // 7. Na samym koñcu zaktualizuj UI
    updateUI();
    //generateGameCode();

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

    /// Back button.
    _backBtn = new menuui::Button();
    _backBtn->setSize({ 180px, 80px });
    _backBtn->position() = { 0.5as, 1as - 25px };
    _backBtn->setLabel()->setRaw("BACK");
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
    _prevBtn->setLabel()->setRaw("< PREV");
    _prevBtn->setCall([this]() { prevStep(); }, nullptr, menuui::Button::Click);
    _navArea->add(_prevBtn);

    /// Next/start button.
    _nextBtn = new menuui::Button();
    _nextBtn->setSize({ 180px, 80px });
    _nextBtn->position() = { 0.8as, 0.5as };
    _nextBtn->setLabel()->setRaw("NEXT >");
    _nextBtn->setCall([this]() { nextStep(); }, nullptr, menuui::Button::Click);
    _navArea->add(_nextBtn);
}

/// Creates game mode selection page.
ui::Element* GameStartMenu::createModePage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "Select Mode");
    title->bounds = { 0, 100px, 1ps, 0 };
    title->align = ui::Text::Center;
    title->pos = ui::Text::Static;
    page->add(title);

    auto makeBtn = [this, page](ui::Dim xOff, const std::string& txt, GameMode val) {
        auto* btn = new menuui::Button();
        btn->setSize({ 320px, 200px });
        btn->position() = { 0.5as + xOff, 0.5as };
        btn->setLabel()->setRaw(txt);
        btn->setCall([this, btn, val]() { selectMode(val, btn); }, nullptr, menuui::Button::Click);
        page->add(btn);
        _modeButtons.push_back(btn);
    };

    makeBtn(-250px, "Singleplayer", Mode_Single);
    makeBtn(250px, "Multiplayer\n(Host)", Mode_Host);

    return page;
}

/// Creates difficulty selection page.
ui::Element* GameStartMenu::createDiffPage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "Select Difficulty");
    title->bounds = { 0, 100px, 1ps, 0 };
    title->align = ui::Text::Center;
    title->pos = ui::Text::Static;
    page->add(title);

    auto makeBtn = [this, page](ui::Dim xOff, const std::string& txt, Difficulty val) {
        auto* btn = new menuui::Button();
        btn->setSize({ 240px, 160px });
        btn->position() = { 0.5as + xOff, 0.5as };
        btn->setLabel()->setRaw(txt);
        btn->setCall([this, btn, val]() { selectDifficulty(val, btn); }, nullptr, menuui::Button::Click);
        page->add(btn);
        _diffButtons.push_back(btn);
    };

    makeBtn(-300px, "EASY", Diff_Easy);
    makeBtn(0px, "MEDIUM", Diff_Medium);
    makeBtn(300px, "HARD", Diff_Hard);

    return page;
}

/// Creates map selection page.
ui::Element* GameStartMenu::createMapPage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "Select Map");
    title->bounds = { 0, 80px, 1ps, 0 };
    title->align = ui::Text::Center;
    title->pos = ui::Text::Static;
    page->add(title);

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

        btn->setLabel()->setRaw(txt);
        btn->setCall([this, btn, val]() { selectMap(val, btn); }, nullptr, menuui::Button::Click);

        page->add(btn);
        _mapButtons.push_back(btn);
    };

    makeBtn(-300px, "Map 1", Map_1);
    makeBtn(0px, "Map 2", Map_2);
    makeBtn(300px, "Map 3", Map_3);

    return page;
}

/// Creates multiplayer lobby page.
ui::Element* GameStartMenu::createLobbyPage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "Lobby Settings");
    title->bounds = { 0, 80px, 1ps, 0 };
    title->align = ui::Text::Center;
    title->pos = ui::Text::Static;
    page->add(title);

    /// Max players section.
    auto* lblPlayers = ui::Text::raw(k_SidebarFont, "Max Players:");
    lblPlayers->bounds = { 0, 200px, 1ps, 0 };
    lblPlayers->align = ui::Text::Center;
    lblPlayers->pos = ui::Text::Static;
    page->add(lblPlayers);

    auto makeCountBtn = [this, page](ui::Dim xOff, int count) {
        auto* btn = new menuui::Button();
        btn->setSize({ 100px, 100px });
        btn->position() = { 0.5as + xOff, 280px };
        btn->setLabel()->setRaw(std::to_string(count));
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

    auto* lblCodeDesc = ui::Text::raw(k_SidebarFont, "Room Code (Share this!):");
    lblCodeDesc->bounds = { 0, 450px, 1ps, 0 };
    lblCodeDesc->align = ui::Text::Center;
    lblCodeDesc->pos = ui::Text::Static;
    page->add(lblCodeDesc);

    auto* lblCode = ui::Text::raw(k_CodeFont, _gameCode);
    lblCode->bounds = { 0, 520px, 1ps, 0 };
    lblCode->align = ui::Text::Center;
    lblCode->pos = ui::Text::Static;
    lblCode->hook([=]() { lblCode->setRaw("#" + _gameCode); });
    page->add(lblCode);

    return page;
}

/// Selects game mode.
void GameStartMenu::selectMode(GameMode mode, menuui::Button* btn) {
    for (auto* b : _modeButtons) if (b != btn) b->deselect();
    bool modeChanged = (_selectedMode != mode);
    _selectedMode = mode;
    btn->select();
    if (modeChanged) {
        //generateGameCode();
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

    // Logika przejœcia z etapu MAPY
    if (_currentStep == STEP_MAP) {
        if (_selectedMode == Mode_Single) {
            // Jeœli Singleplayer -> KONIEC, startujemy grê
            if (_onStartGame) _onStartGame();
            return;
        }
        else {
            // Jeœli Host -> idziemy do ustawieñ Lobby
            _currentStep = STEP_LOBBY;
            updateUI();
            return;
        }
    }

    // Logika przejœcia z ustawieñ LOBBY do POCZEKALNI (tylko dla Hosta)
    if (_currentStep == STEP_LOBBY) {
        _net->host(_maxPlayers, _gameCode);
        _currentStep = STEP_WAITING;

        _pageWaitingLobby->setRoomCode(_gameCode);
        _pageWaitingLobby->setAsHost(true);
        _pageWaitingLobby->updateList({ { "Host (You)", sf::Color::Cyan, true, true } });

        updateUI();
        return;
    }

    // Jeœli jesteœmy w POCZEKALNI i klikamy Start Game
    if (_currentStep == STEP_WAITING) {
        if (_onStartGame) _onStartGame();
        return;
    }

    // Standardowe przejœcie dla kroków MODE i DIFF
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
        // POKAZUJEMY TYLKO LOBBY - Sidebar i NavArea znikaj¹ fizycznie, 
        // bo s¹ w innym dziecku ui::Pages
        _mainSwitcher->show(_pageWaitingLobby);
    }
    else {
        // POKAZUJEMY CA£Y SETUP
        _mainSwitcher->show(_setupContainer);

        // Wewn¹trz setupu pokazujemy odpowiedni¹ pod-stronê
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

    _lblMode->setRaw("1. Mode");
    _lblMode->activate();
    applySettings(_lblMode, (_currentStep == STEP_MODE) ? k_SidebarFontActive : k_SidebarFont);
    
	_lblDiff->setRaw("2. Difficulty");
	_lblDiff->activate();
    applySettings(_lblDiff, (_currentStep == STEP_DIFF) ? k_SidebarFontActive : k_SidebarFont);
    

    _lblMap->setRaw("3. Map");
    _lblMap->activate();
    applySettings(_lblMap, (_currentStep == STEP_MAP) ? k_SidebarFontActive : k_SidebarFont);


    if (_selectedMode == Mode_Host) {
        _lblLobby->activate();
        _lblLobby->setRaw("4. Lobby");
        bool isLobbyActive = (_currentStep == STEP_LOBBY || _currentStep == STEP_WAITING);
        applySettings(_lblLobby, isLobbyActive ? k_SidebarFontActive : k_SidebarFont);
    }
    else {
        // W Singleplayer punkt 4 w ogóle nie istnieje w UI
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

    // Sprawdzamy, czy obecny krok jest ostatnim dla danego trybu
    bool isLastStepForMode = false;
    if (_selectedMode == Mode_Single) {
        isLastStepForMode = (_currentStep == STEP_MAP);
    }
    else {
        // Dla hosta ostatnim krokiem przed wejœciem do poczekalni jest STEP_LOBBY
        isLastStepForMode = (_currentStep == STEP_LOBBY);
    }

    if (isLastStepForMode)
        _nextBtn->setLabel()->setRaw("START GAME");
    else
        _nextBtn->setLabel()->setRaw("NEXT >");

    // Aktywacja przycisku NEXT tylko jeœli dokonano wyboru
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

void GameStartMenu::enterAsJoiner(const std::string& code) {
    _selectedMode = Mode_None;
    _gameCode = code;
    _currentStep = STEP_WAITING;
    updateNavigationButtons();
    updateSidebarLabels();
    _pageWaitingLobby->setRoomCode(_gameCode);
    _pageWaitingLobby->setAsHost(false);
    _pageWaitingLobby->updateList({ {"Connecting...", sf::Color::White, false, false} });

    updateUI();
}
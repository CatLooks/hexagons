#include "menu/gameStartMenu.hpp"
#include "assets.hpp"

static const ui::TextSettings k_HeaderFont = {
    assets::font, 60, sf::Color::White, sf::Color::Black, 3
};
static const ui::TextSettings k_SidebarFont = {
    assets::font, 30, sf::Color(150, 150, 150), sf::Color::Black, 0
};
static const ui::TextSettings k_SidebarFontActive = {
    assets::font, 35, sf::Color::White, sf::Color::Black, 2
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

    buildSidebar();

    // content page container
    _contentPages = new ui::Pages();
    _contentPages->bounds = { 0.25ps, 0, 0.75ps, 0.85ps };
    add(_contentPages);

    // build pages
    _pageMode = createModePage();
    _pageDiff = createDiffPage();
    _pageMap  = createMapPage();

    _contentPages->add(_pageMode);
    _contentPages->add(_pageDiff);
    _contentPages->add(_pageMap);

    buildNavigation();

    _selectedMode = Mode_None;
    _selectedDiff = Diff_None;
    _selectedMap  = Map_None;
    _currentStep  = 0;

    updateUI();
}

/// Builds sidebar UI elements.
void GameStartMenu::buildSidebar() {
    _sidebarBg = new ui::Solid();
    _sidebarBg->color  = sf::Color(15, 15, 25, 255);
    _sidebarBg->bounds = { 0, 0, 0.25ps, 1ps };
    add(_sidebarBg);

    auto addLabel = [this](const std::string& text, ui::Dim yPos) -> ui::Text* {
        auto* lbl = ui::Text::raw(k_SidebarFont, text);
        lbl->bounds = { 0.1as, yPos, 0.8as, 0 };
        lbl->align  = ui::Text::Center;
        lbl->pos    = ui::Text::Static;
        _sidebarBg->add(lbl);
        return lbl;
    };

    _lblMode = addLabel("1. Mode",      0.3ps);
    _lblDiff = addLabel("2. Difficulty", 0.4ps);
    _lblMap  = addLabel("3. Map",       0.5ps);

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
    auto* navArea = makeContainer();
    navArea->bounds = { 0.25ps, 0.85ps, 0.75ps, 0.15ps };
    add(navArea);

    _prevBtn = new menuui::Button();
    _prevBtn->setSize({ 180px, 80px });
    _prevBtn->position() = {0.2as, 0.5as };
    _prevBtn->setLabel()->setRaw("< PREV");
    _prevBtn->setCall([this]() { prevStep(); }, nullptr, menuui::Button::Click);
    navArea->add(_prevBtn);

    _nextBtn = new menuui::Button();
    _nextBtn->setSize({ 180px, 80px });
    _nextBtn->position() = { 0.8as, 0.5as };
    _nextBtn->setLabel()->setRaw("NEXT >");
    _nextBtn->setCall([this]() { nextStep(); }, nullptr, menuui::Button::Click);
    navArea->add(_nextBtn);
}

/// Creates game mode selection page.
ui::Element* GameStartMenu::createModePage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "Select Mode");
    title->bounds = { 0, 100px, 1ps, 0 };
    title->align  = ui::Text::Center;
    title->pos    = ui::Text::Static;
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

    makeBtn(-250px, "Singleplayer",       Mode_Single);
    makeBtn( 250px, "Multiplayer\n(Host)", Mode_Host);

    return page;
}

/// Creates difficulty selection page.
ui::Element* GameStartMenu::createDiffPage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "Select Difficulty");
    title->bounds = { 0, 100px, 1ps, 0 };
    title->align  = ui::Text::Center;
    title->pos    = ui::Text::Static;
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

    makeBtn(-300px, "EASY",   Diff_Easy);
    makeBtn(   0px, "MEDIUM", Diff_Medium);
    makeBtn( 300px, "HARD",   Diff_Hard);

    return page;
}

/// Creates map selection page.
ui::Element* GameStartMenu::createMapPage() {
    auto* page = makeContainer();

    auto* title = ui::Text::raw(k_HeaderFont, "Select Map");
    title->bounds = { 0, 80px, 1ps, 0 };
    title->align  = ui::Text::Center;
    title->pos    = ui::Text::Static;
    page->add(title);

    auto makeBtn = [this, page](ui::Dim xOff, const std::string& txt, MapID val) {
        auto* btn = new menuui::Button();
        btn->setSize({ 260px, 360px });
        btn->position() = { 0.5as + xOff, 0.5as };

        // map preview (centered square crop)
        {
            sf::Vector2u size = assets::map_example.getSize();
            unsigned texW = size.x;
            unsigned texH = size.y;
            unsigned cropSize = texW < texH ? texW : texH;

            int left = static_cast<int>((texW - cropSize) / 2);
            int top  = static_cast<int>((texH - cropSize) / 2);

            sf::IntRect texRect;
            texRect.position.x = left;
            texRect.position.y = top;
            texRect.size.x     = static_cast<int>(cropSize);
            texRect.size.y     = static_cast<int>(cropSize);

            auto* img = new ui::Image(&assets::map_example, texRect);
            img->bounds = { 0.1ps, 0.1ps, 0.8ps, 0.8ps };
            img->tint   = sf::Color::White;
            btn->add(img);
        }

        btn->setLabel()->setRaw(txt);
        btn->setCall([this, btn, val]() { selectMap(val, btn); }, nullptr, menuui::Button::Click);

        page->add(btn);
        _mapButtons.push_back(btn);
    };

    makeBtn(-300px, "Map 1", Map_1);
    makeBtn(   0px, "Map 2", Map_2);
    makeBtn( 300px, "Map 3", Map_3);

    return page;
}

/// Selects a game mode.
void GameStartMenu::selectMode(GameMode mode, menuui::Button* btn) {
    for (auto* b : _modeButtons)
        if (b != btn) b->deselect();

    _selectedMode = mode;
    btn->select();
    updateUI();
}

/// Selects a difficulty.
void GameStartMenu::selectDifficulty(Difficulty diff, menuui::Button* btn) {
    for (auto* b : _diffButtons)
        if (b != btn) b->deselect();

    _selectedDiff = diff;
    btn->select();
    updateUI();
}

/// Selects a map.
void GameStartMenu::selectMap(MapID map, menuui::Button* btn) {
    for (auto* b : _mapButtons)
        if (b != btn) b->deselect();

    _selectedMap = map;
    btn->select();
    updateUI();
}

/// @return Whether the current step has all required selections.
bool GameStartMenu::canProceed() const {
    switch (_currentStep) {
    case 0: return _selectedMode != Mode_None;
    case 1: return _selectedDiff != Diff_None;
    case 2: return _selectedMap  != Map_None;
    default: return false;
    }
}

/// Advances to the next configuration step or starts the game.
void GameStartMenu::nextStep() {
    if (!canProceed()) return;

    if (_currentStep < 2) {
        _currentStep++;
        updateUI();
    } else {
        if (_onStartGame) _onStartGame();
    }
}

/// Returns to the previous configuration step.
void GameStartMenu::prevStep() {
    if (_currentStep > 0) {
        _currentStep--;
        updateUI();
    }
}

/// Updates visible page and sidebar state.
void GameStartMenu::updateUI() {
    ui::Element* page = nullptr;
    switch (_currentStep) {
    case 0: page = _pageMode; break;
    case 1: page = _pageDiff; break;
    case 2: page = _pageMap;  break;
    default: page = nullptr;  break;
    }
    _contentPages->show(page);

    applySettings(_lblMode, _currentStep == 0 ? k_SidebarFontActive : k_SidebarFont);
    applySettings(_lblDiff, _currentStep == 1 ? k_SidebarFontActive : k_SidebarFont);
    applySettings(_lblMap,  _currentStep == 2 ? k_SidebarFontActive : k_SidebarFont);

    updateNavigationButtons();
}

/// Updates navigation button labels and enabled state.
void GameStartMenu::updateNavigationButtons() {
    bool canContinue = canProceed();

    _prevBtn->display = (_currentStep > 0);

    if (_currentStep == 2)
        _nextBtn->setLabel()->setRaw("START GAME");
    else
        _nextBtn->setLabel()->setRaw("NEXT >");

    if (canContinue) {
        _nextBtn->setLabel()->setColor(sf::Color::White);
        _nextBtn->setCall([this]() { nextStep(); }, nullptr, menuui::Button::Click);
    } else {
        _nextBtn->setLabel()->setColor(sf::Color(100, 100, 100));
        _nextBtn->setCall(nullptr, nullptr, menuui::Button::Click);
    }
}

/// Binds back button callback.
void GameStartMenu::bindBack(Action action) {
    _onBack = action;
}

/// Binds start button callback.
void GameStartMenu::bindStart(Action action) {
    _onStartGame = action;
}

/// Draws the game start menu.
void GameStartMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}
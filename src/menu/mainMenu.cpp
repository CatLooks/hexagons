#include "menu.hpp"
#include "assets.hpp" 
#include "ui/anim/linear.hpp"

static const ui::TextSettings title_text_settings = {
    assets::font, 80, sf::Color::White,
    sf::Color::Black, 4
};

static const sf::Color k_MenuBoxColor = sf::Color(20, 20, 25, 230); 
static const sf::Color k_DimmerColor = sf::Color(0, 0, 0, 100);

MainMenu::MainMenu(ui::Layer* layer) : _layer(layer) {
    // 1. Setup the Main Menu Layer
    bounds = { 0, 0, 1ps, 1ps };

    // OPTION A: STATIC IMAGE BACKGROUND
    /*
    _bgImage = new ui::Image(&assets::menu_background_texture); 
    _bgImage->bounds = { 0, 0, 1ps, 1ps };
    add(_bgImage);
    */

    // OPTION B: LIVE GAME BACKGROUND (Dimmer)
    _dimmer = new ui::Solid();
    _dimmer->color = k_DimmerColor;
    _dimmer->bounds = { 0, 0, 1ps, 1ps };
    add(_dimmer);

    // The Translucent Container
    _container = new ui::Solid();
    _container->color = k_MenuBoxColor;
    
    // Size: 500px wide, 700px tall (Adjust as needed)
    _container->bounds = { 0, 0, 500px, 700px }; 
    
    // Position: Exact Center of the screen
    _container->position() = { 0.5as, 0.5as }; 
    
    add(_container);

    // The Title
    // Note: coordinates are relative to the translucent box!
    _title = ui::Text::raw(title_text_settings, "HEXAGONS");
    _title->bounds = { 0, 60px, 1ps, 0 }; // 60px from top of the box
    _title->align = ui::Text::Center;
    _title->pos = ui::Text::Static;
    
    _container->add(_title); 

    // Buttons 
    // We attach these to _container. {0.5as, 0.5as} is now the center of the BOX.
    
    // Play Button (Top)
    _playBtn = new menuui::Button();
    _playBtn->position() = { 0.5as, 0.5as - 100px };
    _playBtn->setLabel()->setRaw("PLAY");
    _playBtn->setCall([this]() { if (_onStart) _onStart(); }, nullptr, menuui::Button::Click);
    
    _container->add(_playBtn);

    // Options Button (Middle)
    _optionsBtn = new menuui::Button();
    _optionsBtn->position() = { 0.5as, 0.5as + 20px };
    _optionsBtn->setLabel()->setRaw("OPTIONS");
    _optionsBtn->setCall([this]() { if (_onOptions) _onOptions(); }, nullptr, menuui::Button::Click);

    _container->add(_optionsBtn);

    // Exit Button (Bottom)
    _exitBtn = new menuui::Button();
    _exitBtn->position() = { 0.5as, 0.5as + 140px };
    _exitBtn->setLabel()->setRaw("EXIT");
    _exitBtn->setCall([this]() { if (_onExit) _onExit(); }, nullptr, menuui::Button::Click);

    _container->add(_exitBtn);

    // Finally, add this menu to the UI layer
    _layer->add(this);
}

void MainMenu::bindStart(Action action) { _onStart = action; }
void MainMenu::bindExit(Action action) { _onExit = action; }
void MainMenu::bindOptions(Action action) { _onOptions = action; }

void MainMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}
#pragma once
#include "ui/layer.hpp"
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "menu/ui/menuButton.hpp"
#include <functional>

class MainMenu : public ui::Element {
public:
    using Action = std::function<void()>;

private:
    ui::Layer* _layer;
    ui::Solid* _bg;
    ui::Text* _title;
    ui::Solid* _dimmer;    // The full screen background
    ui::Solid* _container; // The central translucent box
    ui::Image* _bgImage;   // Optional: a static screenshot (idk if which to use)

    menuui::Button* _playBtn;
    menuui::Button* _optionsBtn; // Added this
    menuui::Button* _exitBtn;

    Action _onStart;
    Action _onExit;
    Action _onOptions; // Added this

public:
    MainMenu(ui::Layer* layer);
    void bindStart(Action action);
    void bindExit(Action action);
    void bindOptions(Action action);

protected:
    void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;
};
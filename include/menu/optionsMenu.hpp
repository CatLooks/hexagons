#pragma once
#include "ui/layer.hpp"
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "menu/ui/menuButton.hpp"
#include <functional>

class OptionsMenu : public ui::Element {
public:
    using Action = std::function<void()>;

private:
    ui::Layer* _layer;
    ui::Solid* _bg;
    ui::Text* _title;

    menuui::Button* _soundBtn;
    menuui::Button* _backBtn;

    // State
    bool _soundEnabled = true;
    Action _onBack;

public:
    OptionsMenu(ui::Layer* layer);

    /// Bind the back button action
    void bindBack(Action action);

protected:
    void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;

    /// Helper to update sound button text
    void updateSoundLabel();
};
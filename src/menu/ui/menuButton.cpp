#include "menu/ui/menuButton.hpp"


namespace menuui {
    static const int k_X = 4;
    static const int k_Y = 2;

    /// Menu button side dimensions.
    const ui::Dim Button::sideX = (float)(64 * k_X);
    const ui::Dim Button::sideY = (float)(64 * k_Y);
    /// Menu button base size.
    const ui::DimVector Button::base = { sideX, sideY };

    /// Menu button texture maps.
    const ui::Panel::Map Button::textures[2] = {
        ui::Panel::Map::rect(&assets::interface, Values::coords(0, 0), { 6, 6 }, 2),
        ui::Panel::Map::rect(&assets::interface, Values::coords(1, 0), { 6, 6 }, 2),
    };

    /// Constructs an empty menu button.
    Button::Button() : ui::Panel(textures[0]) {
        // initialize instance size
        _baseSize = base;
        bounds.size = _baseSize;

        // reset padding
        padding.set(0);

        // event handler
        onEvent([=](const ui::Event& evt) {
            // mouse click
            if (auto data = evt.get<ui::Event::MousePress>()) {
                if (data->button == sf::Mouse::Button::Left) {
                    click();
                    return true;
                };
            };
            // absorb mouse events
            return (bool)evt.mouse();
        });
    };

    /// Sets the button size.
    void Button::setSize(ui::DimVector newSize) {
        _baseSize = newSize;
        bounds.size = newSize;
        if (_tex) {
            _tex->position() = { 0.5as, 0.5as };
        }
    }


    /// Clears the menu button.
    void Button::clear() {
        // remove elements
        ui::Panel::clear();
        ui::Panel::clears();

        // reset pointers
        _tex = nullptr;
        _text = nullptr;
        _sub = nullptr;
        _draw = nullptr;
    };

    /// Adds an image to the menu button.
    void Button::setTexture(const sf::Texture* texture, sf::IntRect map) {
        if (!_tex) {
            _tex = new ui::Image(texture, map);
            _tex->position() = { 0.5as, 0.5as };
            _tex->size() *= (float)k_X;
            adds(_tex);
        }
        else {
            _tex->texture = texture;
            _tex->coords = map;
        };
    };

    /// Adds an extra draw call to the menu button.
    void Button::setDraw(DrawCall call) {
        _draw = call;
    };

    /// Adds a callback to pressed menu button.
    void Button::setCall(Callback press, Callback release, Mode mode) {
        _press = press;
        _release = release;
        _mode = mode;
    };

    /// Emits expand animation.
    ui::Anim* Button::emitExpand() {
        return new ui::AnimVector(&size(), _baseSize, _baseSize * 1.125f, sf::seconds(0.06f));
    };
    /// Emits shrink animation.
    ui::Anim* Button::emitShrink() {
        return new ui::AnimVector(&size(), _baseSize * 1.125f, _baseSize, sf::seconds(0.1f));
    };

    /// Shaker interpolation.
    static float shake(float t) {
        const int shake_count = 2;
        return sinf(shake_count * 2 * (float)M_PI * t);
    };

    /// Forcefully invokes the menu button callback.
    void Button::click() {
        // ignore while animating
        if (animated()) return;

        switch (_mode) {
        case Click:
            if (!display) {
                // animation chain
                ui::Anim* sequence = chain(emitExpand(), emitShrink());
                sequence->setAfter([this]() {
                    this->size() = _baseSize;
                    if (_press) _press();
                    if (_release) _release();
                });
                push(sequence);
            }
            else {
                if (_press) _press();
                if (_release) _release();
            }
            break;

        default:
            // select/toggle mode
            if (!_state) {
                if (_press) _press();
                _state = true;
                _map = textures[1];
                if (!display) push(emitExpand());
            }
            else {
                if (_release) _release();
                _state = false;
                deselect();
            };
            break;
        };
    };

    /// Deselects the button.
    void Button::deselect() {
        if (_state) {
            _state = false;
            _map = textures[0];
            if (!display) push(emitShrink());
        };
    };

    /// Selects the button.
    void Button::select() {
        if (!_state) {
            _state = true;
            _map = textures[1];
            if (!display) push(emitExpand());
        }
    }

    /// Panel text settings.
    const ui::TextSettings panel_text = {
        assets::font, 24, sf::Color::White,
        sf::Color::Black, 2
    };

    /// Adds a label to the menu button.
    ui::Text* Button::setLabel() {
        if (!_text) {
            _text = new ui::Text(panel_text, {});
            _text->bounds = { 0px, 0px, 1ps, 1ps };
            _text->align = ui::Text::Center;
            _text->pos = ui::Text::None;
            add(_text);
        };
        return _text;
    };

    /// Adds a subtitle to the menu button.
    ui::Text* Button::setSubtitle() {
        if (!_sub) {
            _sub = new ui::Text(panel_text, {});
            _sub->bounds = { 0px, -44px, 1ps, 0px };
            _sub->align = ui::Text::N;
            _sub->pos = ui::Text::Static;
            add(_sub);
        };
        return _sub;
    };

    /// Draws the menu button.
    void Button::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
        ui::Panel::drawSelf(target, self);
        if (_draw) _draw(target, self);
    };
}
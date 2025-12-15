#pragma once

// include dependencies
#include "ui.hpp"
#include "assets.hpp"
#include "game/values/shared.hpp"
#include "mathext.hpp"
namespace menuui {
	class Button : public ui::Panel {
	public:
		/// UI draw call.
		///
		/// @param target Render target.
		/// @param self Action box draw area.
		using DrawCall = std::function<void(ui::RenderBuffer& target, sf::IntRect self)>;

		/// Action click callback.
		using Callback = std::function<void()>;

		/// Action button mode.
		enum Mode {
			Click, /// Button is immediately clicked.
			Select /// Button is getting selected during map selection.
		};

	private:
		int _hint = -1;      /// Action hint.
		Mode _mode = Click;   /// Action mode.
		ui::Image* _tex = nullptr; /// Button texture.
		ui::Text* _text = nullptr; /// Button label.
		ui::Text* _sub = nullptr; /// Button subtitle label.
		DrawCall _draw;            /// Extra draw call.
		Callback _press;           /// Button press callback.
		Callback _release;         /// Button release callback.
		bool _state = false;   /// Button state.
		bool _shake = false;   /// Initial shake position.
	public:
		/// Action button side dimentions.
		static const ui::Dim sideX;
		static const ui::Dim sideY;
		/// Action button base size.
		static const ui::DimVector base;

		/// Whether the button acts as a display.
		///
		/// Disables click animations.
		bool display = false;

	protected:
		/// Menu button texture maps.
		///
		/// @param id Whether the button is highlighted.
		static const ui::Panel::Map textures[2];

		/// Draws the action button.
		void drawSelf(ui::RenderBuffer&, sf::IntRect) const override;

	public:
		/// Constructs an empty menu button.
		///
		Button();

		/// Clears the menu button.
		void clear();

		/// Adds an image to the menu button.
		/// 
		/// @param texture Texture reference.
		/// @param map Action texture.
		void setTexture(const sf::Texture* texture, sf::IntRect map);

		/// Adds an extra draw call to the action button.
		/// 
		/// @param call Draw call.
		void setDraw(DrawCall call);
		/// Adds a callback to pressed action button.
		///
		/// @param press Button press callback function.
		/// @param release Button release callback function.
		/// @param mode Button callback mode.
		void setCall(Callback press, Callback release, Mode mode);

		/// Expands the button.
		///
		/// @return Button animation object.
		ui::Anim* emitExpand();
		/// Shrinks the button.
		///
		/// @return Button animation object.
		ui::Anim* emitShrink();
		/// Shakes the button and highlights it in red.
		void errorShake();

		/// Forcefully invokes the action callback.
		void click();
		/// Deselects the button.
		void deselect();

		/// Adds a text label to the action button.
		/// 
		/// @return Pointer to text label.
		ui::Text* setLabel();
		/// Adds a subtitle label to the action button.
		/// 
		/// @return Pointer to text label.
		ui::Text* setSubtitle();
	};
}
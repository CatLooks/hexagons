#pragma once

// include dependencies
#include "ui.hpp"
#include "assets.hpp"
#include "game/map.hpp"
#include "game/draw.hpp"
#include "game/values/interface.hpp"
#include "game/values/skill_values.hpp"

namespace gameui {
	/// Action button element.
	class Action : public ui::Panel {
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
		int _hint       = -1;      /// Action hint.
		Mode _mode      = Click;   /// Action mode.
		ui::Image* _tex = nullptr; /// Action texture.
		ui::Image* _ann = nullptr; /// Annotation texture.
		ui::Text* _text = nullptr; /// Action label.
		ui::Text* _sub  = nullptr; /// Action subtitle label.
		DrawCall _draw;            /// Extra draw call.
		Callback _press;           /// Button press callback.
		Callback _release;         /// Button release callback.
		bool _state     = false;   /// Button state.

	public:
		/// Action button side length.
		static const ui::Dim side;
		/// Action button base size.
		static const ui::DimVector base;

	protected:
		/// Action button texture maps.
		///
		/// @param id Whether the button is highlighted.
		static const ui::Panel::Map textures[2];

		/// Draws the action button.
		void drawSelf(ui::RenderBuffer&, sf::IntRect) const override;

	public:
		/// Constructs an empty action button.
		///
		/// @param hint Hint number (by default no hint).
		Action(int hint = -1);

		/// Clears the action button.
		void clear();

		/// Adds an annotation icon to the action button.
		///
		/// @param ann Annotation type.
		void annotate(SkillDesc::Annotation ann);
		/// Adds an image to the action button.
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
		/// @param mode Action callback mode.
		void setCall(Callback press, Callback release, Mode mode);

		/// Expands the button.
		///
		/// @return Button animation object.
		ui::Anim* emitExpand();
		/// Shrinks the button.
		///
		/// @return Button animation object.
		ui::Anim* emitShrink();

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
};
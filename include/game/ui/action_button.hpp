#pragma once

// include dependencies
#include "ui.hpp"
#include "assets.hpp"
#include "game/map.hpp"
#include "game/values/interface.hpp"
#include "game/draw.hpp"

namespace gameui {
	/// Action button element.
	class Action : public ui::Panel {
	public:
		/// UI draw call.
		///
		/// @param target Render target.
		/// @param self 
		using DrawCall = std::function<void(ui::RenderBuffer& target, sf::IntRect self)>;

	private:
		int _hint       = -1;      /// Action hint.
		ui::Text* _text = nullptr; /// Action label.
		ui::Text* _sub  = nullptr; /// Action subtitle label.
		DrawCall _draw;            /// Extra draw call.

	public:
		/// Action button size.
		static const ui::Dim size;

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

		/// Adds an image to the action button.
		/// 
		/// @param map Action texture.
		void addTexture(sf::IntRect map);

		/// Adds an extra draw call to the action button.
		/// 
		/// @param call Draw call.
		void addDraw(DrawCall call);

		/// Adds a text label to the action button.
		/// 
		/// @return Pointer to text label.
		ui::Text* addLabel();

		/// Adds a subtitle label to the action button.
		/// 
		/// @return Pointer to text label.
		ui::Text* addSubtitle();
	};

	/// Stuff used for hex preview.
	namespace preview {
		extern Map map;         /// Preview "map".
		extern Draw::Tile tile; /// Preview tile draw data.

		/// Draws the preview tile.
		///
		/// @param target Render target.
		/// @param self Draw area.
		void draw(ui::RenderBuffer& target, sf::IntRect self);
	};
};
#pragma once

// include dependencies
#include "resource_bar.hpp"
#include "dev/dev_panel.hpp"

namespace gameui {
	/// Value field element.
	class Field : public ui::Element {
	protected:
		ui::Text*      _label; /// Field label.
		ui::TextField* _field; /// Input field.

	public:
		bool disabled = false; /// Whether the input field is disabled.

		/// Constructs a value field.
		///
		/// @param settings Text settings.
		/// @param label Field label text.
		Field(const ui::TextSettings& settings, const localization::Path& label);

		/// Sets a split between label space and input space.
		/// 
		/// @param a Percent label space takes up.
		/// @param b Percent input space takes up.
		void split(float a, float b);

		/// Adds an icon to the field.
		/// 
		/// @param a Percent label space takes up.
		/// @param b Percent input space takes up.
		/// @param texture Icon texture.
		/// @param coords Icon coordinates.
		/// @param scale Icon scale.
		/// 
		/// @return Icon object.
		ui::Image* icon(float a, float b, const sf::Texture* texture, sf::IntRect coords, float scale = 1.f);

		/// Makes the field numeric.
		/// 
		/// @param digits Max amount of digits.
		/// @param call Field confirm callback.
		void numeric(size_t digits, std::function<void(int result)> call);

		/// Sets input field value.
		/// 
		/// @param text Field value.
		void set(const std::string& text);

		/// Whether any input field is active.
		bool input() const;
	};

	/// Editor resource table element.
	class Table : public dev::Section {
	private:
		Field* _money;  /// Money field.
		Field* _income; /// Income field.
		Field* _peach;  /// Peach field.
		Field* _berry;  /// Berry field.

		ui::Image* _arrow; /// Income arrow icon.

		Region* _reg; /// Attached region.

	public:
		/// Constructs the resource table.
		Table();

		/// Attaches a region to the table.
		/// 
		/// @param reg Region reference.
		void sync(Region* reg);

		/// Whether any input field is active.
		bool input() const;
	};
};
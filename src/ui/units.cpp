#include "ui/units.hpp"

namespace ui {
	/// Linearly interpolates between 2 integers.
	int lerpi(int a, int b, float t) { return a + (int)((b - a) * t); };

	/// Linearly interpolates between 2 floats.
	float lerpf(float a, float b, float t) { return a + (b - a) * t; };

	/// Linearly interpolates between 2 colors.
	sf::Color lerpc(sf::Color a, sf::Color b, float t) { return {
		(uint8_t)lerpi(a.r, b.r, t),
		(uint8_t)lerpi(a.g, b.g, t),
		(uint8_t)lerpi(a.b, b.b, t),
		(uint8_t)lerpf(a.a, b.a, t)
	}; };

	/// Returns dimension's true value.
	int Dim::get(int parent_size, int element_size) const {
		return (int)(px + ps * parent_size + es * element_size);
	};

	/// Constructs a dimension from given values.
	Dim::Dim(float pixels, float parent, float self) : px(pixels), ps(parent), es(self) {};

	/// Constructs a dimension with a base offset.
	Dim Dim::from_px(float px) { return Dim(px, 0.f, 0.f); };
	/// Constructs a dimension with a parent size scalar.
	Dim Dim::from_ps(float ps) { return Dim(0, ps, 0.f); };
	/// Constructs a dimension with an element size scalar.
	Dim Dim::from_me(float me) { return Dim(0, 0.f, me); };

	/// Adds 2 dimensions.
	Dim Dim::operator+(const Dim& oth) const { return { px + oth.px, ps + oth.ps, es + oth.es }; };
	/// Subtracts 2 dimensions.
	Dim Dim::operator-(const Dim& oth) const { return { px - oth.px, ps - oth.ps, es - oth.es }; };
	/// Multiplies the dimension by a factor.
	Dim Dim::operator*(float scale) const { return { px * scale, ps * scale, es * scale }; };
	/// Divides the dimension by an inverted factor.
	Dim Dim::operator/(float scale) const { return { px / scale, ps / scale, es / scale }; };
	/// Inverts the dimension.
	Dim Dim::operator-() const { return { -px, -ps, -es }; };

	/// Adds a dimension.
	Dim& Dim::operator+=(const Dim& oth) { return *this = *this + oth; };
	/// Subtracts a dimension.
	Dim& Dim::operator-=(const Dim& oth) { return *this = *this - oth; };
	/// Multiplies by a factor.
	Dim& Dim::operator*=(float scale) { return *this = *this * scale; };
	/// Divides by a factor.
	Dim& Dim::operator/=(float scale) { return *this = *this / scale; };

	/// Linearly interpolates between 2 dimensions.
	Dim Dim::lerp(Dim a, Dim b, float t) { return a + (b - a) * t; };

	/// Returns dimension vector's true value.
	sf::Vector2i DimVector::get(sf::Vector2i parent, sf::Vector2i element) const {
		return { x.get(parent.x, element.x), y.get(parent.y, element.y) };
	};

	/// Constructs a vector pointing to origin (0, 0).
	DimVector::DimVector() {};
	/// Constructs a vector from specified values.
	/// 
	/// @param x "X" axis dimension.
	/// @param y "Y" axis dimension.
	DimVector::DimVector(Dim x, Dim y) : x(x), y(y) {};
	/// Constructs a vector from an float vector.
	///
	/// @param vec Float vector.
	DimVector::DimVector(sf::Vector2f vec) : x(vec.x), y(vec.y) {};
	/// Constructs a vector from an integer vector.
	///
	/// @param vec Integer vector.
	DimVector::DimVector(sf::Vector2i vec) : DimVector((sf::Vector2f)vec) {};

	/// Adds 2 dimension vectors.
	DimVector DimVector::operator+(const DimVector& oth) const { return { x + oth.x, y + oth.y }; };
	/// Subtracts 2 dimension vectors.
	DimVector DimVector::operator-(const DimVector& oth) const { return { x - oth.x, y - oth.y }; };
	/// Multiplies the vector by a factor.
	DimVector DimVector::operator*(float scale) const { return { x * scale, y * scale }; };
	/// Divides the vector by a factor.
	DimVector DimVector::operator/(float scale) const { return { x / scale, y / scale }; };
	/// Inverts the vector.
	DimVector DimVector::operator-() const { return { -x, -y }; };

	/// Adds a dimension vector.
	DimVector& DimVector::operator+=(const DimVector& oth) { return *this = *this + oth; };
	/// Subtracts a dimension vector.
	DimVector& DimVector::operator-=(const DimVector& oth) { return *this = *this - oth; };
	/// Multiplies by a factor.
	DimVector& DimVector::operator*=(float scale) { return *this = *this * scale; };
	/// Divides by a factor.
	DimVector& DimVector::operator/=(float scale) { return *this = *this / scale; };

	/// @return Vector projection on "X" axis.
	DimVector DimVector::projX() const { return DimVector(x, {}); };
	/// @return Vector projection on "Y" axis.
	DimVector DimVector::projY() const { return DimVector({}, y); };

	/// Linearly interpolates between 2 dimensions.
	DimVector DimVector::lerp(DimVector a, DimVector b, float t) { return a + (b - a) * t; };

	/// Returns dimension rectangle's true value.
	sf::IntRect DimRect::get(sf::IntRect parent) const {
		sf::Vector2i _size = size.get(parent.size, {});
		return { position.get(parent.size, _size) + parent.position, _size };
	};
	/// Returns dimension rectangle's true value.
	sf::IntRect DimRect::get_es(sf::IntRect parent, sf::Vector2i es) const {
		sf::Vector2i _size = size.get(parent.size, es);
		return { position.get(parent.size, _size) + parent.position, _size };
	};

	/// Filling rectangle.
	DimRect DimRect::Fill = { 0px, 0px, 1ps, 1ps };

	/// Constructs an empty rectangle.
	DimRect::DimRect() {};
	/// Constructs a rectangle from vectors.
	/// 
	/// @param position Rectangle position.
	/// @param size Rectangle size.
	DimRect::DimRect(DimVector position, DimVector size) : position(position), size(size) {};
	/// Constructs a rectangle from singular dimensions.
	/// 
	/// @param x Rectangle X position.
	/// @param y Rectangle Y position.
	/// @param width Rectangle width.
	/// @param height Rectangle height.
	DimRect::DimRect(Dim x, Dim y, Dim width, Dim height) : position({ x, y }), size({ width, height }) {};
	/// Constructs a rectangle from a float rectangle.
	/// 
	/// @param rect Float rectangle.
	DimRect::DimRect(sf::FloatRect rect) : DimRect(rect.position.x, rect.position.y, rect.size.x, rect.size.y) {};
	/// Constructs a rectangle from an integer rectangle.
	/// 
	/// @param rect Integer rectangle.
	DimRect::DimRect(sf::IntRect rect) : DimRect((sf::FloatRect)rect) {};

	/// @return Coordinates of top-left corner.
	DimVector DimRect::topLeft() const { return position; };
	/// @return Coordinates of top-left corner.
	DimVector DimRect::topRight() const { return position + size.projX(); };
	/// @return Coordinates of top-left corner.
	DimVector DimRect::bottomLeft() const { return position + size.projY(); };
	/// @return Coordinates of top-left corner.
	DimVector DimRect::bottomRight() const { return position + size; };
};

/// Converts a number of pixels into a dimension.
ui::Dim operator""px(unsigned long long i) { return ui::Dim::from_px((float)i); };
/// Converts a parent size scalar into a dimension.
ui::Dim operator""ps(unsigned long long i) { return ui::Dim::from_ps((float)i); };
/// Converts an element size scalar into a dimension.
ui::Dim operator""me(unsigned long long i) { return ui::Dim::from_me((float)i); };
/// Converts a alignment scalar into a dimension.
ui::Dim operator""as(unsigned long long i) { return ui::Dim(0, (float)i, -(float)i); };

/// Converts a number of pixels into a dimension.
ui::Dim operator""px(long double i) { return ui::Dim::from_px((float)i); };
/// Converts a parent size scalar into a dimension.
ui::Dim operator""ps(long double f) { return ui::Dim::from_ps((float)f); };
/// Converts an element size scalar into a dimension.
ui::Dim operator""me(long double f) { return ui::Dim::from_me((float)f); };
/// Converts a alignment scalar into a dimension.
ui::Dim operator""as(long double f) { return ui::Dim(0, (float)f, -(float)f); };
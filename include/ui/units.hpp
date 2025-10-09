#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace ui {
	/// Linearly interpolates between 2 integers.
	/// 
	/// @param a Starting integer.
	/// @param b Ending integer.
	/// @param t Interpolation progress.
	/// 
	/// @return Interpolated integer.
	int lerpi(int a, int b, float t);

	/// Linearly interpolates between 2 floats.
	/// 
	/// @param a Starting float.
	/// @param b Ending float.
	/// @param t Interpolation progress.
	/// 
	/// @return Interpolated float.
	float lerpf(float a, float b, float t);

	/// Axis dimension.
	/// - Allows to describe position relative to element and its parent size.
	/// - Allows to describe size relative to element size.
	/// 
	/// After recalculation produces: `px + ps * parent_size + es * own_size`.
	struct Dim {
		float px; /// Base value (in pixels).
		float ps; /// Parent size scalar.
		float es; /// Element size scalar.

		/// Returns dimension's true value.
		/// 
		/// @param parent_size Size of parent element.
		/// @param element_size Size of the element.
		/// 
		/// @return Offset in pixels from parent origin.
		int get(int parent_size, int element_size) const;

		/// Constructs a dimension from given values.
		/// 
		/// @param pixels Base value.
		/// @param parent Parent size scalar (0 by default).
		/// @param element Element size scalar (0 by default).
		Dim(float pixels = 0, float parent = 0.f, float self = 0.f);

		/// Constructs a dimension with a base offset.
		static Dim from_px(float px);
		/// Constructs a dimension with a parent size scalar.
		static Dim from_ps(float ps);
		/// Constructs a dimension with an element size scalar.
		static Dim from_es(float es);

		/// Adds 2 dimensions.
		Dim operator+(const Dim& oth) const;
		/// Subtracts 2 dimensions.
		Dim operator-(const Dim& oth) const;
		/// Multiplies the dimension by a factor.
		Dim operator*(float scale) const;
		/// Divides the dimension by an inverted factor.
		Dim operator/(float scale) const;
		/// Inverts the dimension.
		Dim operator-() const;

		/// Adds a dimension.
		Dim& operator+=(const Dim& oth);
		/// Subtracts a dimension.
		Dim& operator-=(const Dim& oth);
		/// Multiplies by a factor.
		Dim& operator*=(float scale);
		/// Divides by a factor.
		Dim& operator/=(float scale);

		/// Linearly interpolates between 2 dimensions.
		/// 
		/// @param a Starting dimension.
		/// @param b Ending dimension.
		/// @param t Interpolation progress.
		/// 
		/// @return Interpolated dimension.
		static Dim lerp(const Dim& a, const Dim& b, float t);
	};

	/// Dimension 2D vector.
	/// Uses axis dimensions to store position or size.
	struct DimVector {
		Dim x; /// "X" axis dimension.
		Dim y; /// "Y" axis dimension.

		/// Returns dimension vector's true value.
		/// 
		/// @param parent Size of parent element.
		/// @param element Size of the element.
		/// 
		/// @return Offset vector from parent origin.
		sf::Vector2i get(sf::Vector2i parent, sf::Vector2i element) const;

		/// Constructs a vector pointing to origin (0, 0).
		DimVector();
		/// Constructs a vector from specified values.
		/// 
		/// @param x "X" axis dimension.
		/// @param y "Y" axis dimension.
		DimVector(Dim x, Dim y);
		/// Constructs a vector from an float vector.
		///
		/// @param vec Float vector.
		DimVector(sf::Vector2f vec);
		/// Constructs a vector from an integer vector.
		///
		/// @param vec Integer vector.
		DimVector(sf::Vector2i vec);

		/// Adds 2 dimension vectors.
		DimVector operator+(const DimVector& oth) const;
		/// Subtracts 2 dimension vectors.
		DimVector operator-(const DimVector& oth) const;
		/// Multiplies the vector by a factor.
		DimVector operator*(float scale) const;
		/// Divides the vector by a factor.
		DimVector operator/(float scale) const;
		/// Inverts the vector.
		DimVector operator-() const;

		/// Adds a dimension vector.
		DimVector& operator+=(const DimVector& oth);
		/// Subtracts a dimension vector.
		DimVector& operator-=(const DimVector& oth);
		/// Multiplies by a factor.
		DimVector& operator*=(float scale);
		/// Divides by a factor.
		DimVector& operator/=(float scale);

		/// @return Vector projection on "X" axis.
		DimVector projX() const;
		/// @return Vector projection on "Y" axis.
		DimVector projY() const;

		/// Linearly interpolates between 2 dimensions.
		/// 
		/// @param a Starting dimension.
		/// @param b Ending dimension.
		/// @param t Interpolation progress.
		/// 
		/// @return Interpolated dimension.
		static DimVector lerp(const DimVector& a, const DimVector& b, float t);
	};

	/// Dimension AABB (axis aligned bounding box).
	/// Used axis dimensions to store position and size of a rectangle.
	struct DimRect {
		DimVector position;  /// Rectangle position (top-left corner).
		DimVector size;      /// Rectangle size (`size.es` will be ignored).

		/// Returns dimension rectangle's true value.
		/// 
		/// @param parent Parent element bounding box.
		/// 
		/// @return Recalculated bounding box.
		sf::IntRect get(sf::IntRect parent) const;

		/// Constructs an empty rectangle.
		DimRect();
		/// Constructs a rectangle from vectors.
		/// 
		/// @param position Rectangle position.
		/// @param size Rectangle size.
		DimRect(DimVector position, DimVector size);
		/// Constructs a rectangle from singular dimensions.
		/// 
		/// @param x Rectangle X position.
		/// @param y Rectangle Y position.
		/// @param width Rectangle width.
		/// @param height Rectangle height.
		DimRect(Dim x, Dim y, Dim width, Dim height);
		/// Constructs a rectangle from a float rectangle.
		/// 
		/// @param rect Float rectangle.
		DimRect(sf::FloatRect rect);
		/// Constructs a rectangle from an integer rectangle.
		/// 
		/// @param rect Integer rectangle.
		DimRect(sf::IntRect rect);

		/// @return Coordinates of top-left corner.
		DimVector topLeft() const;
		/// @return Coordinates of top-left corner.
		DimVector topRight() const;
		/// @return Coordinates of top-left corner.
		DimVector bottomLeft() const;
		/// @return Coordinates of top-left corner.
		DimVector bottomRight() const;
	};
};

/// Converts a number of pixels into a dimension.
ui::Dim operator""px(unsigned long long i);
/// Converts a parent size scalar into a dimension.
ui::Dim operator""ps(unsigned long long i);
/// Converts an element size scalar into a dimension.
ui::Dim operator""es(unsigned long long i);
/// Converts a alignment scalar into a dimension.
ui::Dim operator""as(unsigned long long i);

/// Converts a number of pixels into a dimension.
ui::Dim operator""px(long double i);
/// Converts a parent size scalar into a dimension.
ui::Dim operator""ps(long double f);
/// Converts an element size scalar into a dimension.
ui::Dim operator""es(long double f);
/// Converts a alignment scalar into a dimension.
ui::Dim operator""as(long double f);
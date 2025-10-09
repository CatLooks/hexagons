#pragma once

// include dependencies
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <deque>

namespace ui {
	/// UI rendering statistics.
	struct RenderStats {
		size_t quads     = 0; /// Amount of quads rendered.
		size_t triangles = 0; /// Amount of triangles rendered.
		size_t text      = 0; /// Amount of text lines rendered.
		size_t layers    = 0; /// Amount of layers rendered.

		/// Compiles rendering stats from another structure.
		/// @param oth Other structure.
		void operator|=(const RenderStats& oth);
	};

	/// UI render buffer.
	/// Allows to render a vertex buffer with a single texture.
	class RenderBuffer {
	protected:
		sf::VertexArray      _arr; /// Vertex array.
		sf::RenderStates     _opt; /// Render states.
		std::deque<sf::Text> _txt; /// Rendered text.
		RenderStats          _inf; /// Render stats.

	public:
		/// Constructs a new render buffer.
		/// 
		/// @param texture Render buffer texture.
		/// @param shader Render buffer shader.
		RenderBuffer(const sf::Texture* texture, const sf::Shader* shader = nullptr);

		/// Clears buffer contents.
		void clear();

		/// Queues a triangle for rendering.
		/// 
		/// @param a,b,c Triangle vertices.
		void triangle(sf::Vertex a, sf::Vertex b, sf::Vertex c);

		/// Queues a rectangle for rendering.
		/// 
		/// @param area Rectangle drawing area.
		/// @param texture Rectangle texture coordinates.
		/// @param color Rectangle tint color.
		void quad(sf::IntRect area, sf::IntRect texture, sf::Color color = sf::Color::White);

		/// Queues a text line for rendering.
		/// 
		/// @param text Text object.
		void text(const sf::Text& text);

		/// Renders buffer contents.
		/// 
		/// @param target Render target.
		/// 
		/// @return Render statistics.
		RenderStats draw(sf::RenderTarget& target) const;

		/// @return Buffer rendering states.
		const sf::RenderStates& states() const;
		sf::RenderStates& states();
	};
};
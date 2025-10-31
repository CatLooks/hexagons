#pragma once

// include dependencies
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <list>

namespace ui {
	/// UI rendering statistics.
	struct RenderStats {
		size_t quads     = 0; /// Amount of quads rendered.
		size_t triangles = 0; /// Amount of triangles rendered.
		size_t text      = 0; /// Amount of text lines rendered.
		size_t batches   = 0; /// Amount of batches rendered.
		size_t inters    = 0; /// Amount of intermediate textures rendered.

		/// Compiles rendering stats from another structure.
		/// @param oth Other structure.
		void operator|=(const RenderStats& oth);
	};

	/// UI render buffer.
	/// Allows to render a vertex buffer with a single texture.
	class RenderBuffer {
	protected:
		/// Forwarding index storage.
		struct _FI {
			size_t          vert_count; /// Index of first vertex after forwarding.
			size_t          text_count; /// Index of first text line after forwarding.
			const sf::Texture* texture; /// Current rendering texture.
		};

		std::vector<sf::Vertex>  _arr; /// Vertex array.
		mutable sf::RenderStates _opt; /// Render states.
		const sf::Texture*       _drt; /// Default texture.
		std::list<sf::Text>      _txt; /// Rendered text.
		std::list<_FI>           _fis; /// Forwarding indices.
		RenderStats              _inf; /// Render stats.

	public:
		/// Constructs a new render buffer.
		/// 
		/// @param texture Default render buffer texture.
		/// @param shader Render buffer shader.
		RenderBuffer(const sf::Texture* texture, const sf::Shader* shader = nullptr);

		/// Clears buffer contents.
		void clear();

		/// Queues a triangle for rendering.
		/// 
		/// @param a,b,c Triangle vertices.
		void triangle(sf::Vertex a, sf::Vertex b, sf::Vertex c);
		/// Queues a quad for rendering.
		/// 
		/// @param a,b,c,d Quad vertices (in clock order).
		void quad(sf::Vertex a, sf::Vertex b, sf::Vertex c, sf::Vertex d);
		/// Queues a rectangle for rendering.
		/// 
		/// @param area Rectangle drawing area.
		/// @param texture Rectangle texture coordinates.
		/// @param color Rectangle tint color.
		void quad(sf::IntRect area, sf::IntRect texture, sf::Color color = sf::Color::White);

		/// Queues a text line for rendering.
		/// 
		/// Text lines are drawn after quads & triangles.
		/// 
		/// @param text Text object.
		void text(const sf::Text& text);

		/// Forwards current buffer contents as a single batch.
		///
		/// Normally, all text lines are drawn after quads & triangles.
		/// 
		/// Inserting a forward call, will allow futher quad & triangle calls to be drawn above currently buffered text.
		/// 
		/// @param texture Render buffer texture override.
		void forward();
		void forward(const sf::Texture* texture);

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
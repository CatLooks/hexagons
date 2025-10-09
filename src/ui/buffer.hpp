#pragma once

// include dependencies
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
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
		void operator|=(const RenderStats& oth) {
			quads += oth.quads;
			triangles += oth.triangles;
			text += oth.text;
			layers += oth.layers;
		};
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
		RenderBuffer(const sf::Texture* texture, const sf::Shader* shader = nullptr) : _arr(sf::VertexArray(sf::PrimitiveType::Triangles)) {
			_opt.texture = texture;
			_opt.shader = shader;
		};

		/// Clears buffer contents.
		void clear() {
			_arr.clear();
			_txt.clear();
			_inf = RenderStats();
		};

		/// Queues a triangle for rendering.
		/// 
		/// @param a,b,c Triangle vertices.
		void triangle(sf::Vertex a, sf::Vertex b, sf::Vertex c) {
			_arr.append(a);
			_arr.append(b);
			_arr.append(c);
			_inf.triangles++;
		};

		/// Queues a rectangle for rendering.
		/// 
		/// @param area Rectangle drawing area.
		/// @param texture Rectangle texture coordinates.
		/// @param color Rectangle tint color.
		void quad(sf::IntRect area, sf::IntRect texture, sf::Color color = sf::Color::White) {
			// calculate area rectangle bounds
			float ax0 = (float)area.position.x;
			float ax1 = (float)(area.position.x + area.size.x);
			float ay0 = (float)area.position.y;
			float ay1 = (float)(area.position.y + area.size.y);

			// calculate texture rectangle bounds
			float tx0 = (float)texture.position.x;
			float tx1 = (float)(texture.position.x + texture.size.x);
			float ty0 = (float)texture.position.y;
			float ty1 = (float)(texture.position.y + texture.size.y);

			// calculate quad vertices
			sf::Vertex verts[4] {
				{ { ax0, ay0 }, color, { tx0, ty0 } },
				{ { ax0, ay1 }, color, { tx0, ty1 } },
				{ { ax1, ay1 }, color, { tx1, ty1 } },
				{ { ax1, ay0 }, color, { tx1, ty0 } },
			};
			
			// queue quad triangles
			int indices[6] { 0, 1, 2, 0, 2, 3 };
			for (int idx : indices)
				_arr.append(verts[idx]);
			_inf.quads++;
		};

		/// Queues a text line for rendering.
		/// 
		/// @param text Text object.
		void text(const sf::Text& text) {
			_txt.push_back(text);
			_inf.text++;
		};

		/// Renders buffer contents.
		/// 
		/// @param target Render target.
		/// 
		/// @return Render statistics.
		RenderStats draw(sf::RenderTarget& target) const {
			target.draw(_arr, _opt);
			for (const auto& text : _txt)
				target.draw(text);

			// return layer stats
			RenderStats stats = _inf;
			if (_arr.getVertexCount())
				stats.layers++;
			return stats;
		};

		/// @return Buffer rendering states.
		const sf::RenderStates& states() const { return _opt; };
		sf::RenderStates& states() { return _opt; };
	};
};
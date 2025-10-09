#include "ui/buffer.hpp"

namespace ui {
	/// Compiles rendering stats from another structure.
	void RenderStats::operator|=(const RenderStats& oth) {
		quads += oth.quads;
		triangles += oth.triangles;
		text += oth.text;
		layers += oth.layers;
	};

	/// Constructs a new render buffer.
	RenderBuffer::RenderBuffer(const sf::Texture* texture, const sf::Shader* shader)
		: _arr(sf::VertexArray(sf::PrimitiveType::Triangles)) 
	{
		_opt.texture = texture;
		_opt.shader = shader;
	};

	/// Clears buffer contents.
	void RenderBuffer::clear() {
		_arr.clear();
		_txt.clear();
		_inf = RenderStats();
	};

	/// Queues a triangle for rendering.
	void RenderBuffer::triangle(sf::Vertex a, sf::Vertex b, sf::Vertex c) {
		_arr.append(a);
		_arr.append(b);
		_arr.append(c);
		_inf.triangles++;
	};

	/// Queues a rectangle for rendering.
	void RenderBuffer::quad(sf::IntRect area, sf::IntRect texture, sf::Color color) {
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
		sf::Vertex verts[4]{
			{ { ax0, ay0 }, color, { tx0, ty0 } },
			{ { ax0, ay1 }, color, { tx0, ty1 } },
			{ { ax1, ay1 }, color, { tx1, ty1 } },
			{ { ax1, ay0 }, color, { tx1, ty0 } },
		};

		// queue quad triangles
		int indices[6]{ 0, 1, 2, 0, 2, 3 };
		for (int idx : indices)
			_arr.append(verts[idx]);
		_inf.quads++;
	};

	/// Queues a text line for rendering.
	void RenderBuffer::text(const sf::Text& text) {
		_txt.push_back(text);
		_inf.text++;
	};

	/// Renders buffer contents.
	RenderStats RenderBuffer::draw(sf::RenderTarget& target) const {
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
	const sf::RenderStates& RenderBuffer::states() const { return _opt; };
	sf::RenderStates& RenderBuffer::states() { return _opt; };
};
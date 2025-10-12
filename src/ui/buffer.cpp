#include "ui/buffer.hpp"

namespace ui {
	/// Compiles rendering stats from another structure.
	void RenderStats::operator|=(const RenderStats& oth) {
		quads += oth.quads;
		triangles += oth.triangles;
		text += oth.text;
		batches += oth.batches;
	};

	/// Constructs a new render buffer.
	RenderBuffer::RenderBuffer(const sf::Texture* texture, const sf::Shader* shader) {
		_opt.texture = texture;
		_opt.shader = shader;
	};

	/// Clears buffer contents.
	void RenderBuffer::clear() {
		_arr.clear();
		_txt.clear();
		_fis.clear();
		_inf = RenderStats();
	};

	/// Queues a triangle for rendering.
	void RenderBuffer::triangle(sf::Vertex a, sf::Vertex b, sf::Vertex c) {
		_arr.push_back(a);
		_arr.push_back(b);
		_arr.push_back(c);
		_inf.triangles++;
	};

	/// Queues a quad for rendering.
	void RenderBuffer::quad(sf::Vertex a, sf::Vertex b, sf::Vertex c, sf::Vertex d) {
		_arr.push_back(a);
		_arr.push_back(b);
		_arr.push_back(c);
		_arr.push_back(a);
		_arr.push_back(c);
		_arr.push_back(d);
		_inf.quads++;
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

		// queue quad triangles
		quad(
			{ { ax0, ay0 }, color, { tx0, ty0 } },
			{ { ax0, ay1 }, color, { tx0, ty1 } },
			{ { ax1, ay1 }, color, { tx1, ty1 } },
			{ { ax1, ay0 }, color, { tx1, ty0 } }
		);
		_inf.quads++;
	};

	/// Queues a text line for rendering.
	void RenderBuffer::text(const sf::Text& text) {
		_txt.push_back(text);
		_inf.text++;
	};

	/// Forwards current buffer contents as a single batch.
	void RenderBuffer::forward() {
		// first forward indices
		if (_fis.empty()) {
			_fis.push_back({ _arr.size(), _txt.size() });
		}

		// add offset from last forward indices
		else {
			const _FI& last = _fis.back();
			_fis.push_back({ _arr.size() - last.vert_count, _txt.size() - last.text_count });
		};
	};

	/// Renders buffer contents.
	RenderStats RenderBuffer::draw(sf::RenderTarget& target) const {
		// current batch start position
		size_t vert_idx = 0;
		auto text_it = _txt.begin();

		// batched draw calls
		size_t batch_count = 0;
		for (auto it = _fis.begin(); it != _fis.end(); it++) {
			// draw vertices
			if (it->vert_count > 0) {
				target.draw(_arr.data() + vert_idx, it->vert_count, sf::PrimitiveType::Triangles, _opt);
				vert_idx += it->vert_count;
				batch_count++;
			};

			// draw text
			for (size_t i = 0; i < it->text_count; i++)
				target.draw(*text_it++);
		};

		// draw remaining vertices
		size_t verts_left = _arr.size() - vert_idx;
		if (verts_left > 0) {
			target.draw(_arr.data() + vert_idx, verts_left, sf::PrimitiveType::Triangles, _opt);
			batch_count++;
		};

		// draw remaining text
		while (text_it != _txt.end())
			target.draw(*text_it++);

		// return layer stats
		RenderStats stats = _inf;
		stats.batches = batch_count;
		return stats;
	};

	/// @return Buffer rendering states.
	const sf::RenderStates& RenderBuffer::states() const { return _opt; };
	sf::RenderStates& RenderBuffer::states() { return _opt; };
};
#include "ui/buffer.hpp"

namespace ui {
	/// Default scissor rectangle.
	const sf::FloatRect RenderBuffer::defaultScissor = { {}, {1.f, 1.f} };

	/// Compiles rendering stats from another structure.
	void RenderStats::operator|=(const RenderStats& oth) {
		quads += oth.quads;
		triangles += oth.triangles;
		text += oth.text;
		batches += oth.batches;
		inters += oth.inters;
	};

	/// Constructs a new render buffer.
	RenderBuffer::RenderBuffer(const sf::Shader* shader)
		: _ptl(false) { _opt.shader = shader; };

	/// Clears buffer contents.
	void RenderBuffer::clear() {
		// reset buffers
		_arr.clear();
		_txt.clear();
		_fis.clear();
		_srs.clear();
		_ptl = false;
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
	};

	/// Queues a text line for rendering.
	void RenderBuffer::text(const sf::Text& text) {
		_txt.push_back(text);
		_inf.text++;
	};

	/// Sets next forward scissor area.
	void RenderBuffer::scissor(sf::IntRect area) {
		// intersect scissor with old area
		auto is = sf::FloatRect(
			sf::Vector2f(area.position).componentWiseDiv((sf::Vector2f)_win.size),
			sf::Vector2f(area.size).componentWiseDiv((sf::Vector2f)_win.size)
		).findIntersection(scissor());

		// add new scissor
		_srs.push_back(is ? *is : sf::FloatRect());
	};
	/// Undoes previous scissor area.
	void RenderBuffer::unscissor() {
		if (!_srs.empty())
			_srs.pop_back();
	};

	/// Forwards the buffer.
	void RenderBuffer::forward(const sf::Texture* texture) {
		// buffer extension checks
		if (!_fis.empty()) {
			_FI& last = _fis.back();

			// ignore if scissor rectangle is different
			if (last.scissor != scissor())
				goto skip;

			// check what is drawn in forward
			bool f_vert = _arr.size() != last.vert_idx;
			bool f_text = _txt.size() != last.text_idx;

			// vertex only forward
			if (!f_text && !_ptl && last.texture == texture) {
				last.vert_idx = _arr.size();
				return;
			};

			// text only forward
			if (!f_vert) {
				last.text_idx = _txt.size();
				_ptl = true;
				return;
			};
		};
		skip:
		
		// update text flag
		_ptl = _txt.size() - (_fis.empty() ? 0 : _fis.back().text_idx);

		// add new forward call
		_fis.push_back({
			_arr.size(), _txt.size(),
			texture, scissor()
		});
	};

	/// Renders buffer contents.
	RenderStats RenderBuffer::draw(sf::RenderTarget& target) const {
		// current batch start position
		size_t vert_idx = 0;
		size_t text_idx = 0;
		auto text_it = _txt.begin();

		// batched draw calls
		size_t batch_count = 0;
		for (auto it = _fis.begin(); it != _fis.end(); it++) {
			// get object count
			size_t vert_count = it->vert_idx - vert_idx;
			size_t text_count = it->text_idx - text_idx;

			// set scissor rectangle
			sf::View view = target.getView();
			view.setScissor(it->scissor);
			target.setView(view);

			// draw vertices
			if (vert_count > 0) {
				_opt.texture = it->texture;
				target.draw(_arr.data() + vert_idx, vert_count, sf::PrimitiveType::Triangles, _opt);
				vert_idx = it->vert_idx;
				batch_count++;
			};

			// draw text
			for (size_t i = 0; i < text_count; i++)
				target.draw(*text_it++);
			text_idx = it->text_idx;
		};

#ifdef _DEBUG
		// check for remaining data
		if (_arr.size() - vert_idx > 0) {
			fprintf(stderr, "buffer contains %llu undrawn vertices\n", _arr.size() - vert_idx);
		};
		if (text_it != _txt.end()) {
			size_t counter = 0;
			while (text_it != _txt.end()) {
				text_it++;
				counter++;
			};
			fprintf(stderr, "buffer contains %llu undrawn text labels\n", counter);
		};
#endif

		// return layer stats
		RenderStats stats = _inf;
		stats.batches = batch_count;
		return stats;
	};

	/// @return Buffer rendering states.
	const sf::RenderStates& RenderBuffer::states() const { return _opt; };
	sf::RenderStates& RenderBuffer::states() { return _opt; };

	/// @return Render buffer rendering space.
	const sf::IntRect& RenderBuffer::screen() const { return _win; };

	/// @return Current scissoring rectangle.
	const sf::FloatRect& RenderBuffer::scissor() const {
		return _srs.empty() ? defaultScissor : _srs.back();
	};
};
#pragma once

// include dependencies
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include "element.hpp"
#include <deque>

namespace ui {
	class Interface;

	/// Interface layer element.
	/// Spans the entire window.
	class Layer : public Element {
		friend Interface;

	protected:
		/// Intermediate rendering data.
		struct ir_t {
			sf::RenderTexture tex; /// Render texture.
			sf::IntRect area;      /// Texture blit area.
			sf::Vector2f fact;     /// Local -> global scaling factors.
			sf::Vector2f inv_fact; /// Global -> local scaling factors.
		};

		/// Layer render buffer.
		RenderBuffer _buffer;
		/// Layer intermediate render data.
		std::optional<ir_t> _ir;

	public:
		/// Constructs a new layer.
		/// @param texture Layer rendering texture.
		Layer(const sf::Texture* texture);

		/// Sets new rendering texture.
		void setTexture(const sf::Texture* texture);
		/// Sets new rendering shader.
		void setShader(const sf::Shader* shader);

		/// Configures intermediate rendering.
		///
		/// @param size Intermediate interface size.
		/// @param area Intermediate texture rendering area.
		void setArea(sf::Vector2u size, sf::IntRect area);
		/// Removes an intermediate texture step.
		void removeArea();

		/// Returns layer texture size.
		///
		/// @param window Current window size.
		sf::IntRect view(sf::IntRect window) const;
		/// Maps a screen-space position into a layer position.
		///
		/// @param pos Position in screen space.
		///
		/// @return Position in layer space.
		sf::Vector2i map(sf::Vector2i pos) const;

		/// Renders drawn layer onto render target.
		/// 
		/// @param target Render target.
		/// 
		/// @return Render statistics.
		RenderStats render(sf::RenderTarget& target);
	};

	/// Interface container.
	class Interface {
	private:
		/// Interface layers.
		std::deque<std::unique_ptr<Layer>> layers;
		/// Interface render statistics renderer.
		std::function<void(sf::RenderTarget&, const RenderStats&)> info;
		/// Window rectangle.
		sf::IntRect winRect;
		/// Default view.
		sf::View view;
		/// Update clock.
		sf::Clock upd_clock;
		/// Animation clock.
		sf::Clock anim_clock;

	public:
		/// Creates a new interface layer.
		/// 
		/// @param texture Layer rendering texture, or `nullptr` for solid white texture.
		/// 
		/// @return Reference to new layer.
		Layer* layer(const sf::Texture* texture);

		/// Recalculates interface.
		/// @param windowSize Window size.
		void recalculate(sf::Vector2u windowSize);
		/// Sends an event to interface.
		void event(const sf::Event& evt);
		/// Updates the interface.
		/// @param mouse Mouse position.
		void update(sf::Vector2i mouse);
		/// Draws the interface.
		/// 
		/// @param target Render target.
		void draw(sf::RenderTarget& target);
		/// Updates interface language.
		void translate() const;

		/// Sets rendering statistics rendering callback for the interface.
		/// 
		/// @param target Render target.
		/// @param stats Rendering statistics.
		void setStatDrawCall(std::function<void(sf::RenderTarget& target, const RenderStats& stats)> call);
	};
};
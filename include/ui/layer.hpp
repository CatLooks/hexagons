#pragma once

// include dependencies
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include "element.hpp"
#include <deque>

namespace ui {
	class Interface;

	/// Interface layer element.
	/// Spans the entire window.
	class Layer : public Element {
		friend Interface;

	protected:
		/// Layer render buffer.
		RenderBuffer _buffer;
		/// View override.
		std::optional<sf::View> _view;

		/// Prepare for layer recalculation.
		void onRecalculate() override;

	public:
		/// Constructs a new layer.
		/// @param texture Layer rendering texture.
		Layer(const sf::Texture* texture);

		/// Sets new rendering texture.
		void setTexture(const sf::Texture* texture);
		/// Sets new rendering shader.
		void setTexture(const sf::Shader* shader);
		/// Sets rendering view override.
		/// @param view Optional view override.
		void setView(std::optional<sf::View> view);

		/// Renders drawn layer onto render target.
		/// 
		/// @param target Render target.
		/// @param default_view Default view.
		/// 
		/// @return Rendered triangle count.
		RenderStats render(sf::RenderTarget& target, const sf::View& default_view) const;
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

		/// Sends the event to all interface layers.
		/// @param evt Sent event.
		void send_event(const Event& evt);

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
		/// Send an event to interface.
		void event(const sf::Event& evt);
		/// Updates the interface.
		/// @param mouse Mouse position.
		void update(sf::Vector2i mouse);
		/// Draws the interface.
		/// 
		/// @param target Render target.
		/// @param def_view Default view.
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
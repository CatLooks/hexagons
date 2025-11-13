#pragma once

// include dependencies
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include "element.hpp"
#include <queue>

namespace ui {
	/// Interface layer element.
	/// Spans the entire window.
	class Layer : public Element {
		friend Interface;

	protected:
		/// Intermediate rendering data.
		struct ir_t {
			sf::RenderTexture tex; /// Render texture.
			DimVector size;        /// Texture size.
			DimRect area;          /// Texture blit area.
		};

		/// Layer render buffer.
		RenderBuffer _buffer;
		/// Layer intermediate render data.
		std::optional<ir_t> _ir;

	public:
		/// Constructs a new layer.
		Layer();

		/// Sets new rendering shader.
		void setShader(const sf::Shader* shader);

		/// Configures intermediate rendering.
		///
		/// `es` scalars in `blitArea` will use texture size as scaled value.
		/// 
		/// @param textureSize Intermediate texture size.
		/// @param blitArea Intermediate texture rendering area.
		void setArea(DimVector textureSize, DimRect blitArea);
		/// Removes an intermediate texture step.
		void removeArea();

		/// Returns layer texture size.
		///
		/// @param window Window view rectangle.
		sf::Vector2i size(sf::IntRect window) const;
		/// Returns layer rendering area.
		///
		/// @param window Window view rectangle.
		sf::IntRect view(sf::IntRect window) const;
		/// Maps a screen-space position into a layer position.
		///
		/// @param pos Position in screen space.
		/// @param window Window view rectangle.
		///
		/// @return Position in layer space.
		sf::Vector2i map(sf::Vector2i pos, sf::IntRect window) const;

		/// Renders drawn layer onto render target.
		/// 
		/// @param target Render target.
		/// @param window Window view rectangle.
		/// 
		/// @return Render statistics.
		RenderStats render(sf::RenderTarget& target, sf::IntRect window);
	};

	/// Interface container.
	class Interface {
	private:
		/// Layer array.
		using _Context = std::deque<std::unique_ptr<Layer>>;

		/// Interface contexts.
		std::list<_Context> _contexts;
		/// Default interface context.
		_Context _default;
		/// Current interface context.
		_Context* _ctx = &_default;
		/// Next interface context.
		_Context* _next = &_default;
		/// Interface modification lock.
		bool _lock = false;

		/// Interface render statistics renderer.
		std::function<void(sf::RenderTarget&, const RenderStats&)> _info;
		/// Window rectangle.
		sf::IntRect _win_rect;
		/// Default view.
		sf::View _view;
		/// Update clock.
		sf::Clock _upd_clock;
		/// Animation clock.
		sf::Clock _anim_clock;
		/// Clear color.
		sf::Color _clear_color;

	public:
		/// Context handle type.
		class Context {
			friend Interface;

			/// Context reference.
			_Context* ref;
			/// Default constructor.
			Context(_Context* ref) : ref(ref) {};
		};

		/// Switches an interface context.
		/// 
		/// This method will fail if the interface has been locked.
		/// 
		/// @param ctx Context handle.
		void setContext(const Context& ctx);
		/// Switches to a default interface context.
		/// 
		/// This method will fail if the interface has been locked.
		/// 
		/// @return Context handle.
		void defaultContext();
		/// Generates a new interface context.
		/// 
		/// This method will fail if the interface has been locked.
		/// 
		/// After generation, switches to the context.
		/// 
		/// @return Context handle.
		Context newContext();
		/// Switches to an interface context during the next frame.
		/// 
		/// @param ctx Context handle.
		void switchContext(const Context& ctx);
		/// Locks the interface from immediate context switching.
		void lock();

		/// Creates a new interface layer.
		/// 
		/// @param value Layer object (can be overwritten with a derivative of `ui::Layer`).
		///
		/// @return Reference to new layer.
		Layer* layer(Layer* value = new Layer());

		/// Recalculates interface.
		/// @param windowSize Window size.
		void recalculate(sf::Vector2u windowSize);
		/// Sends an event to interface.
		void event(const sf::Event& evt);
		/// Sends an event queue to interface.
		///
		/// Pops all events from the queue in the process.
		void eventq(std::queue<sf::Event>& queue);
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
		void statDraw(std::function<void(sf::RenderTarget& target, const RenderStats& stats)> call);
		/// Sets clearing color.
		void clearColor(sf::Color color);
	};
};
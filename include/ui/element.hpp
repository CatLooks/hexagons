#pragma once

// include dependencies
#include <SFML/System/Time.hpp>
#include "units.hpp"
#include "event.hpp"
#include "buffer.hpp"
#include "anim/base.hpp"
#include <functional>
#include <deque>
#include <list>
#include <memory>

namespace ui {
	/// Contains thickness for borders in 4 directions.
	struct Borders {
		int left   = 0; /// Thickness of left border.
		int right  = 0; /// Thickness of right border.
		int top    = 0; /// Thickness of top border.
		int bottom = 0; /// Thickness of bottom border.

		/// Sets thickness in all directions.
		/// @param value Thickness value.
		void set(int value);
		/// Sets thickness in horizontal directions.
		/// @param value Thickness value.
		void setHorizontal(int value);
		/// Sets thickness in vertical directions.
		/// @param value Thickness value.
		void setVertical(int value);

		/// Reserves space for borders in a rectangle.
		/// @return New shrinked rectangle.
		sf::IntRect apply(sf::IntRect rect) const;
	};

	/// Base UI element object.
	class Element {
	public:
		/// Event handler function type.
		/// 
		/// @param evt Event object.
		/// 
		/// @return Whether the event has been absorbed.
		using EventHandler = std::function<bool(const Event& evt)>;
		/// Update handler function type.
		/// 
		/// @param delta Time elapsed since last frame.
		using UpdateHandler = std::function<void(const sf::Time& delta)>;
		/// Static event handler function type.
		using StaticHandler = std::function<void()>;

		/// Element children iterator.
		///
		/// Iterates over system items, then public children.
		class iter {
			friend Element;

		public:
			/// Iterator value type.
			using value_type = std::unique_ptr<Element>;
			/// Iterator join operand type.
			using iter_type = std::list<std::unique_ptr<Element>>::const_iterator;

		private:
			iter_type _it0;   /// First list iterator.
			iter_type _it1;   /// Second list iterator.
			iter_type _split; /// First iterator end.

			/// Constructs an iterator object.
			/// 
			/// @param a First iterator.
			/// @param split First iterator end.
			/// @param b Second iterator.
			iter(iter_type a, iter_type split, iter_type b);

		public:
			/// Returns current iterator item.
			const value_type& operator*() const;
			/// Returns current iterator item.
			const value_type* operator->() const;

			/// Advances to next value.
			void operator++();
			/// Advances to next value.
			void operator++(int);

			/// Checks if 2 iterators point to the same item.
			/// 
			/// @param oth Other iterator.
			bool operator==(const iter& oth) const;

			/// Checks if 2 iterators point to different items.
			/// 
			/// @param oth Other iterator.
			bool operator!=(const iter& oth) const;
		};

		/// @return Starting iterator to all children.
		iter begin() const;
		/// @return Ending iterator to all children.
		iter end() const;

	private:
		/// System children.
		std::list<std::unique_ptr<Element>> _system;
		/// Element's children.
		std::list<std::unique_ptr<Element>> _elements;
		/// Element's parent.
		Element* _parent = nullptr;

		sf::IntRect _outerRect; /// Draw area from parent container.
		sf::IntRect _rect;      /// Draw area after padding.
		sf::IntRect _innerRect; /// Draw area after padding & margin.

		/// Event handler list.
		std::list<EventHandler> _handle_list;
		/// Pre-recalculation update handler list.
		std::list<UpdateHandler> _recalc_list;
		/// Post-recalculation update handler list.
		std::list<UpdateHandler> _update_list;
		/// Active animator list.
		std::list<std::unique_ptr<Anim>> _anims;

		/// Whether the element was being hovered over.
		bool _hover_old = false;
		/// Whether the element is being hovered over.
		bool _hover_now = false;

		/// Whether the element is active.
		bool _active = true;

	public:
		bool infinite     = false; /// Whether element's bounding box is infinite.
		bool ignore       = false; /// Whether the element ignores events.
		bool transparent  = false; /// Whether mouse hover events can pass through the element.
		bool scissor      = false; /// Whether to cut off pixels drawn outside of element's bounds.
		bool event_scissor = true; /// Whether to discard events out-of-bounds from propagating to children.

		DimRect bounds = DimRect::Fill; /// Element bounding box.
		Borders padding;                /// Element padding.
		Borders margin;                 /// Element margin.

	protected:
		/// Draws the element.
		/// 
		/// @param target Render buffer.
		/// @param self Element drawing area.
		virtual void drawSelf(RenderBuffer& target, sf::IntRect self) const;
		/// Draws element children.
		/// 
		/// @param target Render buffer.
		virtual void drawChildren(RenderBuffer& target) const;

		/// Runs when the UI language gets changed.
		virtual void onTranslate();
		/// Runs when the element gets activated.
		virtual void onActivate();
		/// Runs when the element gets deactivated.
		virtual void onDeactivate();
			
	public:
		/// Virtual destructor.
		virtual ~Element();

		/// Adds new element as a child.
		/// 
		/// Pointer to element must be owning.
		/// @param element New child element.
		void add(Element* element);
		/// Removes a child element if present.
		/// @param element Old child element.
		void remove(Element* element);
		/// Replaces old element by a new one.
		/// 
		/// If the old element is not found, the new one will be added with `add(Element*)`.
		/// 
		/// @param old Old element pointer.
		/// @param repl New element pointer.
		void replace(Element* old, Element* repl);
		/// Moves the element to be the last in element queue.
		///
		/// @param element Child element pointer.
		void to_front(Element* element);
		/// Moves the element to be the first in element queue.
		///
		/// @param element Child element pointer.
		void to_back(Element* element);
		/// Removes all child elements.
		void clear();

	protected:
		/// Adds new element as a system item.
		/// 
		/// Pointer to element must be owning.
		/// @param element New system element.
		void adds(Element* element);
		/// Removes a system element if present.
		/// @param element Old child element.
		void removes(Element* element);
		/// Replaces old system element by a new one.
		/// 
		/// If the old element is not found, the new one will be added with `adds(Element*)`.
		/// 
		/// @param old Old element pointer.
		/// @param repl New element pointer.
		void replaces(Element* old, Element* repl);
		/// Removes all system elements.
		void clears();

	public:
		/// Pushes a new animation.
		/// 
		/// Pointer to the animator must be owning.
		/// @param anim New animator.
		void push(Anim* anim);
		/// @return Whether the element has any animations running.
		bool animated() const;
		/// Chains second animation to first.
		/// 
		/// Second animation will start immediately when the first one finishes.
		/// 
		/// @param first First animation object.
		/// @param second Second animation object.
		/// 
		/// @return First animation object.
		Anim* chain(Anim* first, Anim* second);

		/// Recalculates draw area for the element.
		/// 
		/// Order of updates:
		/// 
		/// 1. pre-recalculation updates
		/// 2. animation update
		/// 3. element recalculation
		/// 4. children recalculation
		/// 
		/// @param delta Time elapsed since last frame.
		/// @param parent Parent bounding box.
		void recalculate(const sf::Time& delta, sf::IntRect parent);
		/// Recalculates draw area after main recalculation.
		///
		/// Recalculation uses `0` as delta; animations will not be ticked.
		///
		/// This method will not work if the element does not have a parent.
		void recalculate();
		/// Draws the element and its children.
		/// @param target Render buffer.
		void draw(RenderBuffer& target) const;
		/// Updates element and its children.
		/// @param delta Time elapsed since last frame.
		void update(const sf::Time& delta);

		/// Emits an event to the element.
		/// 
		/// @param evt Event object.
		/// 
		/// @return Whether the event has been absorbed.
		bool event(Event evt);
		/// Handles the event.
		/// 
		/// @param evt Event object.
		/// 
		/// @return Whether the event has been absorbed.
		bool handle(Event evt);
		/// Processes mouse hovering events.
		/// 
		/// @param pos Mouse position.
		/// 
		/// @return Whether the hover has been absorbed.
		bool hover(sf::Vector2i pos);

		/// Attaches an event handler.
		/// 
		/// Event handler receives an event object.
		/// 
		/// If the handler returns `true`, the event stops propagating.
		/// @param handler Event handler.
		void onEvent(const EventHandler& handler);
		/// Attaches an update handler.
		/// 
		/// Update handler receives the time delta.
		/// @param handler Update handler.
		void onUpdate(const UpdateHandler& handler);
		/// Attaches a recalculation handler.
		/// 
		/// Recalculation handler receives the time delta.
		/// @param handler Update handler.
		void onRecalculate(const UpdateHandler& handler);

		/// Updates UI language.
		void translate();
		/// Activates the element.
		void activate();
		/// Deactivates the element.
		void deactivate();
		/// @return Whether the element is active.
		bool active() const;

		/// @return Element position.
		DimVector& position();
		const DimVector& position() const;
		/// @return Element size.
		DimVector& size();
		const DimVector& size() const;

		/// @return Element bounding rectangle (available after recalculation).
		sf::IntRect rect() const;

		/// Returns element's children.
		/// This does not include system items (use `begin()` and `end()` for all children).
		const std::list<std::unique_ptr<Element>>& children() const;
		/// @return Element's parent.
		Element* parent() const;

	protected:
		/// Returns element's system children.
		/// This does not include public items (use `begin()` and `end()` for all children).
		const std::list<std::unique_ptr<Element>>& childrens() const;
	};
};
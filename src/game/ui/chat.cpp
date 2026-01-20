#include "game/ui/chat.hpp"

namespace gameui {
	/// Constructs the game chat.
	Chat::Chat(ui::Dim input, ui::Dim msg, size_t len, size_t max):
		_line(msg), _max(max), _active(false)
	{
		infinite = true;

		// create message container
		_list = new ui::Element;
		_list->scissor = true;
		_list->bounds = { 0px, 0as, 1ps, 1ps - input };
		add(_list);

		// create text input
		_field = new ui::TextField({ .texture = nullptr }, Values::chat_text, sf::Color::White);
		_field->color = Values::dimTint;
		_field->bounds = { 0px, 1ps, 1ps, input };
		_field->margin.set(6);
		_field->margin.top = 0;
		add(_field);

		// setup text input
		_field->input.attachTextConfirm([=](const sf::String& raw) {
			// get utf-8 string
			auto exp = raw.toUtf8();
			std::string text;
			{
				text.reserve(exp.size());
				for (uint8_t c : exp)
					text.push_back(c);
			};

			// clear chat
			_field->input.clear();

			// print message in chat
			if (_call) _call(text);
		});

		// limit text length
		_field->input.attachValidation([=](const sf::String& text, char32_t) {
			return text.getSize() < len;
		});

		// add text input deselect
		_field->onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::KeyPress>()) {
				if (data->key == sf::Keyboard::Key::Escape) {
					// inhibit unfocus autohide
					_active = false;

					// unfocus the text input
					hide();
					return true;
				};
			};
			return false;
		});
		_field->onFocus([=](bool focused) {
			// hide chat if unfocusing an active chat
			if (!focused && _active)
				hide();
		});

		// auto message clean-up routine
		onUpdate([=](const sf::Time&) {
			auto it = _msg.begin();
			while (it != _msg.end()) {
				// check if message has expired
				bool expired = (*it)->life > sf::seconds(10);

				// advance to next message
				if (!expired) it++;
				else {
					(*it)->fadeOut();    // remove message box
					it = _msg.erase(it); // remove message from queue
				};
			};
		});
	};

	/// Constructs a message element.
	Chat::Message::Message(ui::Dim height): lines(1), move(nullptr) {
		// configure message box
		bounds = { -1ts, 1as - 6px, 1ps - 12px, height };
		color = Values::dimTint;
		padding.setHorizontal(4);

		// construct author label
		auth = new ui::Text(Values::chat_text, "chat.author");
		auth->autosize = true;
		auth->pos = ui::Text::Static;
		add(auth);

		// construct content label
		cont = ui::Text::raw(Values::chat_text, "");
		cont->autosize = true;
		cont->pos = ui::Text::Static;
		add(cont);

		// message update routine
		onUpdate([=](const sf::Time& delta) {
			// update message lifetime
			life += delta;

			// set content label x-position
			cont->position() = auth->position() + auth->size().projX();

			// set message box size
			size().x = cont->position().x + cont->size().x + (float)(padding.left + padding.right);
			recalculate();
		});
	};

	/// Fades in the message.
	void Chat::Message::fadeIn() {
		auto* anim = new ui::AnimDim(&position().x, -1ts, 6px, sf::seconds(0.5f));
		anim->ease = ui::Easings::sineOut;
		parent()->push(anim);
	};

	/// Fades out the message.
	void Chat::Message::fadeOut() {
		auto* anim = new ui::AnimDim(&position().x, 6px, -1ts, sf::seconds(0.5f));
		anim->ease = ui::Easings::quadOut;
		anim->setAfter([this, parent = parent()]() {
			// remove message from parent
			parent->remove(this);
		});
		parent()->push(anim);
	};

	/// Moves up the message.
	void Chat::Message::moveUp(ui::Dim offset) {
		// get current position
		auto& y = position().y;

		// check if an animation is already running
		if (move) {
			// cancel previous animation
			move->cancel();
			ui::Dim y = move->end_state() - offset;

			// start fast move animation
			move = ui::AnimDim::to(&position().y, y, sf::seconds(0.25f));
			move->ease = ui::Easings::sineOut;
		}
		else {
			// create slow move animation
			ui::Dim y = position().y - offset;
			move = ui::AnimDim::to(&position().y, y, sf::seconds(0.25f));
			move->ease = ui::Easings::sine;
		};
		move->setAfter([=]() {
			// clear move animation reference
			move = nullptr;
		});
		push(move);
	};

	/// Shows the chat input field.
	void Chat::show() {
		_active = true;

		// create animation
		auto* anim = ui::AnimDim::to(&_field->position().y, 1as, sf::seconds(0.16f));
		anim->ease = ui::Easings::quadOut;
		push(anim);

		// focus input field
		_field->focus(true);
	};

	/// Hides the chat input field.
	void Chat::hide() {
		_active = false;

		// create animation
		auto* anim = ui::AnimDim::to(&_field->position().y, 1ps, sf::seconds(0.16f));
		anim->ease = ui::Easings::quadIn;
		push(anim);

		// unfocus input field
		_field->focus(false);
	};

	/// Writes a message to the chat.
	void Chat::print(const std::string& author, sf::Color color, const std::string& text) {
		// clear first buffered message if too many
		if (_msg.size() >= _max) {
			_msg.front()->fadeOut();
			_msg.pop_front();
		};

		// construct new message
		auto* msg = new Message(_line);
		_list->add(msg);
		
		// set message contents
		// @todo chat line break-up
		msg->auth->param("auth", author);
		msg->auth->setColor(color);
		msg->cont->setRaw(text);

		// update line count
		msg->lines = std::count(text.cbegin(), text.cend(), '\n') + 1;

		// shift lines up
		for (const auto& msg : _msg)
			msg->moveUp(_line * (float)msg->lines);

		// add new message to queue
		_msg.push_back(msg);
		msg->fadeIn();

		// recalculate new message
		_list->recalculate();
	};

	/// Attaches a chat message send callback.
	void Chat::attach(std::function<void(const std::string&)> callback) {
		_call = callback;
	};

	/// Checks whether the chat input field is active.
	bool Chat::active() const {
		return _field->focused();
	};
};
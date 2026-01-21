#include "game/sync/messages.hpp"
#include <cassert>
#include <variant>

int main() {
	Messages::Event evtSel = Messages::Select{ 7u, true };
	assert(std::holds_alternative<Messages::Select>(evtSel));
	const auto& sel = std::get<Messages::Select>(evtSel);
	assert(sel.id == 7u && sel.turn);

	Messages::Event evtChat = Messages::Chat{ "hello" };
	assert(std::holds_alternative<Messages::Chat>(evtChat));
	const auto& chat = std::get<Messages::Chat>(evtChat);
	assert(chat.text == "hello");
	return 0;
}
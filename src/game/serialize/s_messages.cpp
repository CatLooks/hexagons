#include "game/serialize/messages.hpp"
#include "game/serialize/map.hpp"

namespace Serialize {
	/// Writes player description to the packet.
	sf::Packet& operator<<(sf::Packet& packet, const Messages::Player& plr) {
		packet << plr.name;
		packet << (uint8_t)plr.team;
		return packet;
	};

	/// Reads player description from the packet.
	sf::Packet& operator>>(sf::Packet& packet, Messages::Player& plr) {
		packet >> plr.name;
		plr.team = static_cast<Region::Team>(from<uint8_t>(packet));
		return packet;
	};

	/// Writes an event message to the packet.
	void encodeMessage(sf::Packet& packet, const Messages::Event& evt) {
		// game initialization
		if (auto* data = std::get_if<Messages::Init>(&evt)) {
			packet << (uint8_t)E_Init;
			packet << data->temp;
			encodeVec<Messages::Player>(packet, data->players);
		};
		// game over
		if (auto* data = std::get_if<Messages::End>(&evt)) {
			packet << (uint8_t)E_End;
			packet << data->id;
		};
		// player select
		if (auto* data = std::get_if<Messages::Select>(&evt)) {
			packet << (uint8_t)E_Select;
			packet << data->id;
			packet << data->turn;
		};
		// player ignore
		if (auto* data = std::get_if<Messages::Ignore>(&evt)) {
			packet << (uint8_t)E_Ignore;
			packet << data->id;
		};
		// chat message
		if (auto* data = std::get_if<Messages::Chat>(&evt)) {
			packet << (uint8_t)E_Chat;
			packet << data->text;
		};
	};

	/// Reads an event message from the packet.
	std::optional<Messages::Event> decodeMessage(sf::Packet& packet) {
		// get message type
		auto type = from<uint8_t>(packet);
		if (type >= E_Count) return std::nullopt;

		// read message data
		switch (type) {
			// game initialization
			case E_Init: return Messages::Init
			{
				.temp = from<Template>(packet),
				.players = decodeVec<Messages::Player>(packet)
			};
			// game over
			case E_End: return Messages::End
			{
				.id = from<uint32_t>(packet)
			};
			// player select
			case E_Select: return Messages::Select
			{
				.id = from<uint32_t>(packet),
				.turn = from<bool>(packet)
			};
			// player ignore
			case E_Ignore: return Messages::Ignore
			{
				.id = from<uint32_t>(packet)
			};
			// chat message
			case E_Chat: return Messages::Chat
			{
				.text = from<std::string>(packet)
			};
		};
		return {};
	};
};

#include "game/sync/state.hpp"
#include "game/values/hex_values.hpp"

/// Constructs a game state object.
GameState::GameState(Mode mode, Adapter* adapter):
	_adapter(adapter), _mode(mode), _state(Init),
	_map(nullptr), _chat(nullptr)
{
	
};

/// Adds a player update callback.
void GameState::updateCallback(std::function<void(bool enabled)> callback) {
	_call = callback;
};

/// Adds a new player to the game.
void GameState::addPlayer(const Player& player) {
	_plr.push_back(player);
};

/// Updates object references.
void GameState::setRefs(Map* map, gameui::Chat* chat) {
	_map = map;
	_chat = chat;
};

/// Sends a message to chat.
void GameState::message(const std::string& text) {
	// get "you" name
	std::string you = assets::lang::locale.req("chat.you").get({});

	// display message in chat
	_chat->print(you, Values::hex_colors[team()], text);

	// broadcast the message
	_adapter->send(Adapter::Chat{ .text = text });
};

/// Initializes the game.
void GameState::init() {
	// quit if no players
	if (_plr.empty()) {
		_state = Quit;
		return;
	};

	// reset player index
	_idx = 0;
	_state = _idx == _adapter->id ? Play : Wait;
	_call(_idx == _adapter->id);
};

/// Attempts to finish a move.
bool GameState::finish() {
	// ignore if not making a move
	if (_state != Play) return false;

	// transmit move list
	auto list = _map->history.list();
	_adapter->send_list({ list, _adapter->id });

	// reset history
	_map->history.clear();

	// select next player
	next();
	return true;
};

/// Advances the game to the next player.
void GameState::next() {
	if (_mode == Host) {
		_idx++;
		_clock.restart();
		if (_idx >= _plr.size()) {
			// reset player counter
			_idx = 0;
			_turn++;

			// tick the map
			auto list = logic::turn(_map);

			// transmit changes
			_adapter->send_list(list);
		};

		// select next player
		_adapter->send(Adapter::Select{ .id = _idx });

		// update player state
		_state = _idx == _adapter->id ? Play : Wait;
		if (_call) _call(_idx == _adapter->id);
	};
};

/// Processes incoming messages.
void GameState::tick() {
	// incoming move lists
	while (auto data = _adapter->recv_list()) {
		// ignore own packets
		if (data->id == _adapter->id) continue;

		// retransmit packets if needed
		if (_mode == Host) {
			// ignore if inactive player
			if (data->id != _idx) {
				_adapter->send(Adapter::Ignore{ .id = data->id });
				continue;
			};

			// retransmit to others
			_adapter->send_list({ data->value, data->id });
		};

		// sync game map
		for (const auto& move : data->value)
			move->apply(_map);

		// select next player
		next();
	};

	// incoming events
	while (auto data = _adapter->recv()) {
		// reset timer on player selection
		if (std::holds_alternative<Adapter::Select>(data->value))
			_clock.restart();

		// ignore own packets
		if (data->id == _adapter->id) continue;

		// retransmit to others
		if (_mode == Host)
			_adapter->send(*data);

		// process the event
		proc(*data);
	};
};

/// Processes a single event.
void GameState::proc(const Adapter::Packet<Adapter::Event>& event) {
	// display chat message
	if (auto* data = std::get_if<Adapter::Chat>(&event.value)) {
		// get author info
		const Player* player = event.id >= _plr.size() ? nullptr : &_plr[event.id];

		// create chat message
		_chat->print(
			player ? player->name : "???",
			player ? Values::hex_colors[player->team] : sf::Color::White,
			data->text
		);
		return;
	};

	printf("event proc\n");
};

/// Returns current player info.
const GameState::Player* GameState::player() const {
	return _plr.empty() ? nullptr : &_plr[_idx];
};

/// Returns next player info.
const GameState::Player* GameState::nextPlayer() const {
	if (_plr.empty())
		return nullptr;
	return _idx + 1 >= _plr.size()
		? &_plr[0] : &_plr[_idx + 1];
};

/// Returns current turn time.
sf::Time GameState::turnTime() const {
	return _clock.getElapsedTime();
};

/// Returns local player team.
Region::Team GameState::team() const {
	return _plr.empty() ? Region::Unclaimed : _plr[_adapter->id].team;
};

/// Returns current turn number.
size_t GameState::turn() const {
	return _turn;
};
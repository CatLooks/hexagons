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
void GameState::addPlayer(const Messages::Player& player) {
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
	_adapter->send(Messages::Chat{ .text = text });
};

/// Updates gameplay state.
void GameState::update() {
	_state = _idx == _adapter->id ? Play : Wait;
	_call(_idx == _adapter->id);
};

/// Locks gameplay state.
void GameState::lock() {
	_state = Wait;
	_call(false);
};

/// Initializes the game.
void GameState::init() {
	// quit if no players
	if (_plr.empty()) {
		_state = Quit;
		return;
	};

	// send initialization packet
	_adapter->send(Messages::Init{
		.players = _plr
	});

	// select first player
	_idx = 0;
	_adapter->send(Messages::Select{ .id = _idx });
	update();
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
	// lock game until a selection is received
	lock();

	// next player logic
	if (_mode == Host) {
		bool turn = false;

		// increment player index
		if (++_idx >= _plr.size()) {
			// increment turn number
			_idx = 0;
			_turn++;

			// tick the map
			auto list = logic::turn(_map);

			// transmit changes
			_adapter->send_list(list);
			turn = true;
		};
		_clock.restart();
		update();

		// select next player
		_adapter->send(Messages::Select{ .id = _idx, .turn = turn });
	};
};

/// Processes incoming messages.
void GameState::tick() {
	// incoming move lists
	while (auto data = _adapter->recv_list()) {
		// ignore own packets
		if (data->id == _adapter->id) continue;

		// retransmit packets if host
		if (_mode == Host) {
			// ignore if inactive player
			if (data->id != _idx) {
				_adapter->send(Messages::Ignore{ .id = data->id });
				continue;
			};

			// retransmit move list to others
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
void GameState::proc(const Adapter::Packet<Messages::Event>& event) {
	// game initialization
	if (auto* data = std::get_if<Messages::Init>(&event.value)) {
		// store player list
		_plr = data->players;
		_turn = 0;
		_idx = 0;

		// get adapter index
		// @todo

		// wait for player selection
		lock();
		return;
	};

	// player selections
	if (auto* data = std::get_if<Messages::Select>(&event.value)) {
		// update player index
		_idx = data->id;
		if (data->turn) _turn++;
		update();

		// reset turn time
		_clock.restart();
		return;
	};

	// display chat message
	if (auto* data = std::get_if<Messages::Chat>(&event.value)) {
		// get author info
		const Player* player = event.id >= _plr.size() ? nullptr : &_plr[event.id];

		// create chat message
		_chat->print(
			player ? player->name : assets::lang::locale.req("chat.unknown").get({}),
			player ? Values::hex_colors[player->team] : sf::Color(32, 32, 32),
			data->text
		);
		return;
	};

	printf("unknown event\n");
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
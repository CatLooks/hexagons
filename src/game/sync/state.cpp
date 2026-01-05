#include "game/sync/state.hpp"

/// Constructs a game state object.
GameState::GameState(Mode mode, Adapter* adapter):
	_adapter(adapter), _map(nullptr), _mode(mode), _state(Init)
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

/// Updates map object reference.
void GameState::setMap(Map* map) {
	_map = map;
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
		if (_idx >= _plr.size()) {
			// reset player counter
			_idx = 0;

			// tick the map
			//std::unique_ptr<Move> move(new Moves::GameTurn);
			//printf("@todo\n");
			//move->apply(_map);
			auto history = logic::turn(_map);

			// transmit changes
			_adapter->send_list(history.list());
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
		// ignore own packets
		if (data->id == _adapter->id) continue;

		// retransmit to others
		if (_mode == Host)
			_adapter->send(data->value);

		// process the event
		proc(*data);
	};
};

/// Processes a single event.
void GameState::proc(const Adapter::Packet<Adapter::Event>& event) {
	printf("event proc\n");
};

/// Returns current player info.
const GameState::Player* GameState::player() const {
	return _plr.empty() ? nullptr : &_plr[_idx];
};

/// Returns local player team.
Region::Team GameState::team() const {
	return _plr.empty() ? Region::Unclaimed : _plr[_adapter->id].team;
};
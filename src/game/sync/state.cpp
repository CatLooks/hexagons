#include "game/sync/state.hpp"
#include "game/values/hex_values.hpp"

/// Constructs a game state object.
GameState::GameState(Mode mode, Adapter* adapter):
	_adapter(adapter), _mode(mode), _state(Init),
	_map(nullptr), _chat(nullptr), _splash(nullptr)
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
void GameState::setRefs(Map* map, gameui::Chat* chat, gameui::Splash* splash) {
	_map = map;
	_chat = chat;
	_splash = splash;
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
	// update game state
	_state = _idx == _adapter->id ? Play : Wait;
	_call(_idx == _adapter->id);

	// display "your turn" splash if local player is selected
	if (_state == Play) {
		_splash->queue("splash.your_turn", Values::hex_colors[team()]);
		_splash->display();
	};
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

		// send error message to chat
		_chat->print(
			assets::lang::locale.req("chat.host").get({}),
			Values::host_color,
			assets::lang::locale.req("chat.no_players").get({})
		);
		return;
	};

	// send initialization packet
	_adapter->send(Messages::Init{
		.temp = Template::generate(_map),
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

	// select next player
	next();
	return true;
};

/// Finishes the game.
void GameState::over(size_t id) {
	// stop game
	_state = Quit;
	_call(false);
	_clock.stop();

	// get player data
	auto& player = _plr[id];

	// splash message arguments
	_splash->args["name"] = player.name;
	_splash->args["team"] = Values::hex_names[player.team];

	// display splash message
	_splash->queue("splash.game_over");
	_splash->frame();
	_splash->queue("splash.victor.a");
	_splash->queue("space");
	_splash->queue("splash.victor.b", Values::hex_colors[player.team]);
	_splash->queue("space");
	_splash->queue("splash.victor.c");
	_splash->queue("space");
	_splash->queue("splash.victor.d", Values::hex_colors[player.team]);
	_splash->queue("space");
	_splash->queue("splash.victor.e");
	_splash->display();
};

/// Advances the game to the next player.
void GameState::next() {
	// lock game until a selection is received
	lock();

	// next player logic
	if (_mode == Host) {
		bool turn = false;

		// reset move history
		_map->history.clear();

		// tick & transmit player regions
		auto list = logic::turn(_map, player()->team);
		_adapter->send_list(list);

		// check for game over
		auto count = logic::count(_map);
		auto team = logic::win(count);
		if (team != Region::Unclaimed) {
			// find player index
			uint32_t id = 0;
			for (const auto& player : _plr)
				if (player.team == id)
					break;

			// switch to spectator state
			over(id);

			// send game over packet
			_adapter->send(Messages::End{ .id = id });
			return;
		};

		// increment player index
		if (++_idx >= _plr.size()) {
			// increment turn number
			_idx = 0;
			_turn++;

			// tick & transmit the map
			auto list = logic::global(_map);
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
				_adapter->send(Messages::Ignore{
					.id = data->id,
					.now = _idx
				});
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
		// construct game map
		data->temp.construct(_map);

		// store player list
		_plr = data->players;
		_turn = 0;
		_idx = 0;

		// wait for player selection
		lock();
		return;
	};

	// game over
	if (auto* data = std::get_if<Messages::End>(&event.value)) {
		over(data->id);
		return;
	};

	// player selections
	if (auto* data = std::get_if<Messages::Select>(&event.value)) {
		// update player index
		_idx = data->id;
		if (data->turn) _turn++;
		update();

		// reset history if local player's turn
		if (_state == Play)
			_map->history.clear();

		// reset turn time
		_clock.restart();
		return;
	};

	// player ignore
	if (auto* data = std::get_if<Messages::Ignore>(&event.value)) {
		// ignore if not the target player
		if (data->id != _adapter->id) return;

		// display error message
		_chat->print(
			assets::lang::locale.req("chat.host").get({}),
			Values::host_color,
			assets::lang::locale.req("chat.ignore").get({})
		);
		return;
	};

	// display chat message
	if (auto* data = std::get_if<Messages::Chat>(&event.value)) {
		// get author info
		const Player* player = event.id >= _plr.size() ? nullptr : &_plr[event.id];

		// create chat message
		_chat->print(
			player ? player->name : assets::lang::locale.req("chat.unknown").get({}),
			player ? Values::hex_colors[player->team] : Values::unknown_color,
			data->text
		);
		return;
	};

	// unknown event
	_chat->print(
		assets::lang::locale.req("chat.host").get({}),
		Values::host_color,
		assets::lang::locale.req("chat.bad_event").get({
			{ "id", ext::str_int(event.value.index()) }
		})
	);
};

/// Returns current player info.
const GameState::Player* GameState::player() const {
	return _plr.empty() ? nullptr : &_plr[_idx];
};

/// Returns next player info.
const GameState::Player* GameState::nextPlayer() const {
	if (_plr.empty())
		return nullptr;
	return _idx + 1ull >= _plr.size()
		? &_plr[0] : &_plr[_idx + 1ull];
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
uint32_t GameState::turn() const {
	return _turn;
};
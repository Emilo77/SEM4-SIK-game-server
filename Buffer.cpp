#include "Buffer.h"

static inline void check_direction(uint8_t direction) {
	/* Jeżeli typ Direction jest niepoprawny, rzucamy wyjątek */
	if (direction > Direction::Left) {
		throw InvalidMessage();
	}
}

static inline void check_client_message_type(uint8_t type) {
	/* Jeżeli typ ServerMessageToClientType jest niepoprawny, rzucamy wyjątek */
	if (type > ClientMessageToServerType::Move) {
		throw InvalidMessage();
	}
}

void Buffer::check_if_message_incomplete(size_t size) const {
	/* Jeżeli czytana wiadomość wykroczyłaby poza koniec danych, rzucamy wyjątek*/
	if (read_index + size > end_of_data_index) {
		throw IncompleteMessage();
	}
}

uint16_t Buffer::convert_to_send(uint16_t number) { return htobe16(number); }

uint32_t Buffer::convert_to_send(uint32_t number) { return htobe32(number); }

uint16_t Buffer::convert_to_receive(uint16_t number) { return be16toh(number); }

uint32_t Buffer::convert_to_receive(uint32_t number) { return be32toh(number); }

void Buffer::insert_raw(const string &str) {
	size_t size = str.size();
	memcpy(&send_buffer[send_index], str.c_str(), size);
	send_index += size;
}

void Buffer::insert(uint8_t number) {
	memcpy(&send_buffer[send_index], &number, sizeof(number));
	send_index += sizeof(number);
}

void Buffer::insert(uint16_t number) {
	number = convert_to_send(number);
	memcpy(&send_buffer[send_index], &number, sizeof(number));
	send_index += sizeof(number);
}

void Buffer::insert(uint32_t number) {
	number = convert_to_send(number);
	memcpy(&send_buffer[send_index], &number, sizeof(number));
	send_index += sizeof(number);
}

void Buffer::insert(Position &position) {
	insert(position.x);
	insert(position.y);
}

void Buffer::insert(const string &str) {
	insert((uint8_t) str.size());
	insert_raw(str);
}

void Buffer::insert(Player &player) {
	insert(player.name);
	insert(player.address);
}

void Buffer::receive_raw(string &str, size_t str_size) {
	try {
		check_if_message_incomplete(str_size);
		str = {&receive_buffer[read_index], str_size};
		read_index += str_size;

	} catch (IncompleteMessage &e) {
		throw e;
	}
}

void Buffer::receive(uint8_t &number) {
	try {
		size_t size = sizeof(number);
		check_if_message_incomplete(size);
		memcpy(&number, &receive_buffer[read_index], size);
		read_index += size;

	} catch (IncompleteMessage &e) {
		throw e;
	}
}

void Buffer::receive(uint16_t &number) {
	try {
		size_t size = sizeof(number);
		check_if_message_incomplete(size);
		memcpy(&number, &receive_buffer[read_index], size);
		read_index += size;
		number = convert_to_receive(number);

	} catch (IncompleteMessage &e) {
		throw e;
	}
}

void Buffer::receive(uint32_t &number) {
	try {
		size_t size = sizeof(number);
		check_if_message_incomplete(size);
		memcpy(&number, &receive_buffer[read_index], size);
		read_index += size;
		number = convert_to_receive(number);

	} catch (IncompleteMessage &e) {
		throw e;
	}
}

void Buffer::receive(string &str) {
	try {
		uint8_t string_size = 0;
		receive(string_size);
		receive_raw(str, string_size);

	} catch (IncompleteMessage &e) {
		throw e;
	}
}

void Buffer::insert_list_player_ids(std::vector<player_id_t> &ids) {
	insert((uint32_t) ids.size());
	for (auto &id: ids) {
		insert(id);
	}

}

void Buffer::insert_list_positions(std::vector<Position> &positions) {
	insert((uint32_t) positions.size());
	for (auto &position: positions) {
		insert(position);
	}
}

void Buffer::insert_map_players(std::map<player_id_t, Player> &players) {
	insert((uint32_t) players.size());
	for (auto &player: players) {
		insert(player.first);
		insert(player.second);
	}
}

void Buffer::insert_map_scores(std::map<player_id_t, score_t> &scores) {
	insert((uint32_t) scores.size());
	for (auto &scoresElement: scores) {
		insert(scoresElement.first);
		insert(scoresElement.second);
	}
}

void Buffer::insert_bomb_placed(struct BombPlaced &bomb_placed) {
	insert(bomb_placed.bomb_id);
	insert(bomb_placed.position);
}

void Buffer::insert_bomb_exploded(struct BombExploded &bomb_exploded) {
	insert(bomb_exploded.bomb_id);
	insert_list_player_ids(bomb_exploded.robots_destroyed);
	insert_list_positions(bomb_exploded.blocks_destroyed);
}

void Buffer::insert_player_moved(struct PlayerMoved &player_moved) {
	insert(player_moved.player_id);
	insert(player_moved.position);
}

void Buffer::insert_block_placed(struct BlockPlaced &block_placed) {
	insert(block_placed.position);
}

void Buffer::insert_event(Event &event) {
	switch (event.type) {
		case BombPlaced:
			insert_bomb_placed(std::get<BombPlaced>(event.data));
			break;
		case BombExploded:
			insert_bomb_exploded(std::get<BombExploded>(event.data));
			break;
		case PlayerMoved:
			insert_player_moved(std::get<PlayerMoved>(event.data));
			break;
		case BlockPlaced:
			insert_block_placed(std::get<BlockPlaced>(event.data));
			break;
	}
}

void Buffer::insert_list_events(std::vector<Event> &vector) {
	try {
		insert((uint32_t) vector.size());
		for (auto &event: vector) {
			insert_event(event);
		}
	} catch (IncompleteMessage &e) {
		throw e;
	}
}

void Buffer::insert_hello(struct Hello &hello) {
	insert(hello.server_name);
	insert(hello.players_count );
	insert(hello.size_x );
	insert(hello.size_y );
	insert(hello.game_length );
	insert(hello.explosion_radius );
	insert(hello.bomb_timer );
}

void Buffer::insert_accepted_player(struct AcceptedPlayer &accepted_player) {
	insert(accepted_player.player_id);
	insert(accepted_player.player);
}

void Buffer::insert_game_started(struct GameStarted &game_started) {
	insert_map_players(game_started.players);
}

void Buffer::insert_turn(struct Turn &turn) {
	insert(turn.turn_number);
	insert_list_events(turn.events);
}

void Buffer::insert_game_ended(struct GameEnded &game_ended) {
	insert_map_scores(game_ended.scores);
}

void Buffer::initialize(size_t size) {
	receive_buffer.resize(size, 0);
	send_buffer.resize(size, 0);
}

void Buffer::adapt_size() {
	if (shift_index + MAX_PACKAGE_SIZE > receive_buffer.size()) {
		receive_buffer.resize(receive_buffer.size() + MAX_PACKAGE_SIZE, 0);
	}
}
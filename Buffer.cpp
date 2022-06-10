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

uint8_t Buffer::receive_u8() {
	uint8_t number;
	try {
		size_t size = sizeof(number);
		check_if_message_incomplete(size);
		memcpy(&number, &receive_buffer[read_index], size);
		read_index += size;

	} catch (IncompleteMessage &e) {
		throw e;
	}
	return number;
}

std::string Buffer::receive_string() {
	std::string str;
	try {
		uint8_t string_size = receive_u8();
		receive_raw(str, string_size);

	} catch (IncompleteMessage &e) {
		throw e;
	}
	return str;
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
	insert((uint8_t) event.type);
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
	insert((uint32_t) vector.size());
	for (auto &event: vector) {
		insert_event(event);
	}
}

void Buffer::insert_hello(struct Hello &hello) {
	insert(hello.server_name);
	insert(hello.players_count);
	insert(hello.size_x);
	insert(hello.size_y);
	insert(hello.game_length);
	insert(hello.explosion_radius);
	insert(hello.bomb_timer);
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
	std::cerr << std::endl;
}

void Buffer::insert_game_ended(struct GameEnded &game_ended) {
	insert_map_scores(game_ended.scores);
}

size_t Buffer::insert_ServerMessage(ServerMessage &message) {
	/* Resetujemy index do wysyłania. */
	reset_send_index();
	/* Wstawiamy typ wiadomości do bufora. */
	insert((uint8_t) message.type);
	/* W zależności od typu wstawiamy resztę wiadomości. */
	switch (message.type) {
		case Hello:
			insert_hello(std::get<struct Hello>(message.data));
			break;
		case AcceptedPlayer:
			insert_accepted_player(
					std::get<struct AcceptedPlayer>(message.data));
			break;
		case GameStarted:
			insert_game_started(std::get<struct GameStarted>(message.data));
			break;
		case Turn:
			insert_turn(std::get<struct Turn>(message.data));
			break;
		case GameEnded:
			insert_game_ended(std::get<struct GameEnded>(message.data));
			break;
	}
	/* Zwracamy ilość oktetów do wysłania.*/
	return get_send_size();
}

ClientMessage Buffer::receive_ClientMessage(size_t received_size,
                                            std::optional<player_id_t>
                                            player_id) {
	/* Resetujemy index do czytania danych z bufora. */
	reset_read_index();

	/* Zwiększamy index końca danych o otrzymaną ilość danych. */
	end_of_data_index += received_size;

	/* Inicjujemy potrzebne zmienne do przechowania wiadomości. */
	auto clientMessage = std::optional<ClientMessage>();
	uint8_t message;
	uint8_t possible_direction;
	std::variant<std::string, Direction> data;

	try {
		/* Odbieramy i sprawdzamy typ wiadomości */
		message = receive_u8();
		check_client_message_type(message);

		switch ((ClientMessageToServerType) message) {
			case Join:
				data = receive_string();
				clientMessage.emplace(Join, data);
				break;
			case PlaceBomb:
				clientMessage.emplace(PlaceBomb, data);
				break;
			case PlaceBlock:
				clientMessage.emplace(PlaceBlock, data);
				break;
			case Move:
				possible_direction = receive_u8();
				check_direction(possible_direction);
				data = (Direction) possible_direction;
				clientMessage.emplace(Move, data);
				break;
		}

	} catch (IncompleteMessage &e) {
		/* Jeżeli wiadomość okazała się niekompletna, przesuwamy
        * index odbierania danych. Przekazujemy wyjątek dalej. */
		set_shift(end_of_data_index);
		throw e;

	} catch (InvalidMessage &e) {
		/* Jeżeli wiadomość okazała się niepoprawna, przekazujemy wyjątek
        * dalej. */
		throw e;
	}

	/* Dodajemy id gracza do wiadomości. */
	clientMessage->player_id = player_id;

	/* Jeżeli udało się wczytać wiadomość z danych z bufora,
	 * przesuwamy index końca danych i index odbierania danych. */
	end_of_data_index -= get_read_size();
	set_shift(end_of_data_index);

	/* Przesuwamy dane o ilość, jaką wczytaliśmy. */
	for (size_t i = 0; i < end_of_data_index; i++) {
		receive_buffer[i] = receive_buffer[i + get_read_size()];
	}

	return clientMessage.value();
}

void Buffer::initialize(size_t send_size, size_t receive_size) {
	/* Inicjujemy bufory na odpowiednią wielkość. */
	send_buffer.resize(send_size, 0);
	receive_buffer.resize(receive_size, 0);
}

void Buffer::adapt_size() {
	/* Rozszerzamy bufor do wystarczających rozmiarów. */
	if (shift_index + MID_SIZE > receive_buffer.size()) {
		receive_buffer.resize(receive_buffer.size() + MID_SIZE, 0);
	}
}

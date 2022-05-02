#include "Buffer.h"

#include <cstring>

using std::string;


template<typename T>
T Buffer::convert_to_send(T number) {
	switch (sizeof(T)) {
		case 1:
			return number;
		case 2:
			return htobe16(number);
		case 4:
			return htobe32(number);
		default:
			return htobe64(number);
	}
}

template<typename T>
T Buffer::convert_to_receive(T number) {
	switch (sizeof(T)) {
		case 1:
			return number;
		case 2:
			return be16toh(number);
		case 4:
			return be32toh(number);
		default:
			return be64toh(number);
	}
}

template<typename T>
void Buffer::insert(T number) {
	int size = sizeof(T);
	number = convert_to_send(number);
	memcpy(buffer + send_index, &number, size);
	send_index += size;
}

void Buffer::insert(const string &str) {
	size_t size = str.size();
	memcpy(buffer + send_index, str.c_str(), size);
	send_index += size;
}

void Buffer::insert(Position &position) {
	uint16_t x_to_send = convert_to_send(position.x);
	uint16_t y_to_send = convert_to_send(position.y);
	memcpy(buffer + send_index, &x_to_send, sizeof(x_to_send));
	send_index += sizeof(x_to_send);
	memcpy(buffer + send_index, &y_to_send, sizeof(y_to_send));
	send_index += sizeof(x_to_send);
}

template<typename T>
//todo ograniczenie na T, że musi być primitive
void Buffer::insert(std::list<T> &list) {
	insert((uint32_t) list.size());
	for (T &list_element: list) {
		insert(list_element);
	}
}

void Buffer::insert(Player &player) {
	insert(player.get_id());
	insert((uint8_t) player.name.size());
	insert(player.name);
	insert((uint8_t) player.address.size());
	insert(player.address);
}

void Buffer::insert(BombPlaced &e) {
	insert(e.bomb_id);
	insert(e.position);
}

void Buffer::insert(BombExploded &e) {
	insert(e.bomb_id);
	insert(e.robots_destroyed);
	insert(e.blocks_destroyed);
}

void Buffer::insert(PlayerMoved &e) {
	insert(e.player_id);
	insert(e.position);
}

void Buffer::insert(BlockPlaced &e) {
	insert(e.position);
}

template<typename T>
void Buffer::receive_number(T &number) {
	int size = sizeof(T);
	memcpy(&number, buffer + read_index, size);
	read_index += size;
	number = convert_to_receive(number);
}

void Buffer::receive_string(string &str, size_t str_size) {
	str = {buffer + read_index, str_size};
}

void Buffer::insert_hello(Parameters &parameters) {
	reset_read_index();
	insert((uint8_t) parameters.server_name.size());
	insert(parameters.server_name);
	insert(parameters.players_count);
	insert(parameters.size_x);
	insert(parameters.size_y);
	insert(parameters.game_length);
	insert(parameters.explosion_radius);
	insert(parameters.bomb_timer);
}

void Buffer::insert_accepted_player(Player &player) {
	reset_read_index();
	insert(player);
}

void Buffer::insert_game_started(std::map<player_id_t, Player> &map) {
	reset_read_index();
	insert((uint32_t) map.size());
	for(auto &element: map) {
		insert(element.second);
	}
}

void Buffer::insert_turn(uint16_t turn_number, std::list<Event> &events) {
	reset_read_index();
	insert(turn_number);
	//todo wstawić listę
}

void Buffer::insert_game_ended(std::map<player_id_t, score_t> &map) {
	reset_read_index();
	insert((uint32_t) map.size());
	for(auto &element: map) {
		insert(element.first);
		insert(element.second);
	}
}

//jeżeli niepoprawna instrukcja, zwraca pusty string
string Buffer::receive_join(size_t received_size) {
	reset_read_index();
	string name;
	uint8_t name_size;
	receive_number(name_size);
	if (name_size != received_size - 2) { //niepoprawna instrukcja
		return name;
	}
	receive_string(name, name_size);
	return name;
}

Direction Buffer::receive_move() {
	reset_read_index();
	uint8_t direction;
	receive_number(direction);
	if (direction > 3) {
		return Direction::InvalidDirection;
	}
	return (Direction) direction;
}

//todo: stworzyć listę eventów i je tam dodawać
//todo: rozwiązać problem z dziedziczeniem po klasie Event

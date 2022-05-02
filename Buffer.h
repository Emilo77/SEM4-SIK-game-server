#ifndef ZADANIE02_BUFFER_H
#define ZADANIE02_BUFFER_H

#define BUFFER_SIZE 65507

#include "Utils.h"

class Buffer {
private:
	template<typename T>
	T convert_to_send(T number);

	template<typename T>
	T convert_to_receive(T number);

	template<typename T>
	void insert(T number);

	void insert(const std::string &str);

	void insert(Position &position);

	template<typename T>
	//todo ograniczenie na T, że musi być primitive

	void insert(std::list<T> &list);

	void insert(BombPlaced &e);

	void insert(BombExploded &e);

	void insert(PlayerMoved &e);

	void insert(BlockPlaced &e);

	template<typename T>
	void receive_number(T &number);

	void receive_string(std::string &str, size_t str_size);

// we begin from 1, because we already know the value of buffer[0]- message_id
	void reset_read_index() { read_index = 1; }

	void reset_send_index() { send_index = 0; }

public:

	void insert_turn();

	std::string receive_join(size_t received_size);

	Direction receive_move();

	size_t get_size() const { return send_index; }

	char get_message_id() { return buffer[0]; }

	char *get() { return buffer; }

private:
	char buffer[BUFFER_SIZE]{};
	size_t send_index{0};
	size_t read_index{1};
};


#endif //ZADANIE02_BUFFER_H

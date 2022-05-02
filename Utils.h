#ifndef ZADANIE02_EVENT_H
#define ZADANIE02_EVENT_H

#include <cstdint>
#include <string>
#include <list>
#include <bits/sigaction.h>
#include <csignal>

using bomb_id_t = uint32_t;

class Random {
	uint32_t random;
public:
	explicit Random(uint32_t seed) : random(seed) {}

	uint32_t generate() {
		uint32_t r = random;
		random = (random * 279410273) % 4294967291;
		return r;
	}
};

enum Direction {
	Up = 0,
	Right = 1,
	Down = 2,
	Left = 3,
	InvalidDirection = 4,
};

enum ClientMessage {
	Join = 0,
	PlaceBomb = 1,
	PlaceBlock = 2,
	Move = 3,
	Invalid = 4,
	// przy castowaniu na enum, dowolna inna wartość niż <1,4>
	// stanie siże zerem, czyli Invalid
};

struct ClientMessageStruct {
	ClientMessage message;
	std::string name{};
	Direction direction{};

	ClientMessageStruct(ClientMessage message)
			: message(message) {}

	ClientMessageStruct(ClientMessage message, std::string &name)
			: message(message),
			  name(name) {}

	ClientMessageStruct(ClientMessage message, Direction direction)
			: message(message),
			  direction(direction) {}
};


struct Position {
	uint16_t x;
	uint16_t y;

	Position(uint16_t x, uint16_t y) : x(x), y(y) {}

	bool operator==(const Position &rhs) const {
		return x == rhs.x &&
		       y == rhs.y;
	}

	bool operator!=(const Position &rhs) const {
		return !(x == rhs.x &&
		         y == rhs.y);
	}
};

std::pair<int,int> direction_to_pair(Direction &direction) {
	switch(direction) {
		case Up:
			return {0, 1};
		case Right:
			return {1, 0};
		case Down:
			return {0, -1};
		case Left:
			return {-1, 0};
		case InvalidDirection:
			return {69, 69}; //todo potem zmienić
	}
}

class Bomb {
	uint32_t bomb_id;
	uint16_t timer;
	Position position;

public:
	bool operator<(const Bomb &rhs) const {
		return bomb_id < rhs.bomb_id;
	}

	void tic() {
		timer--;
	}

	bool will_explode() {
		return timer == 0;
	}

	bomb_id_t get_id() {
		return bomb_id;
	}

	Position get_position() {
		return position;
	}
};

class Event {
};

class BombPlaced : public Event {
public:
	uint32_t bomb_id;
	Position position;
};

class BombExploded : public Event {
public:
	uint32_t bomb_id;
	std::list<uint8_t> robots_destroyed;
	std::list<Position> blocks_destroyed;
};

class PlayerMoved : public Event {
public:
	uint8_t player_id;
	Position position;
};

class BlockPlaced : public Event {
public:
	Position position;
};

#endif //ZADANIE02_EVENT_H

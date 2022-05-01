#ifndef ZADANIE02_EVENT_H
#define ZADANIE02_EVENT_H

#include <cstdint>
#include <string>
#include <list>

struct Random {
	uint32_t random;

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
};

struct Player {
	std::string name;
	std::string address;
};

struct Position {
	uint16_t x;
	uint16_t y;

	Position(uint16_t x, uint16_t y) : x(x), y(y) {}
};

struct Bomb {
	time_t explosion_time;
	Position position;
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

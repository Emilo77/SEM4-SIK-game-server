#ifndef ZADANIE02_EVENT_H
#define ZADANIE02_EVENT_H

#include <cstdint>
#include <string>
#include <list>
#include <csignal>
#include <variant>
#include <vector>

using player_id_t = uint8_t;
using bomb_id_t = uint32_t;
using score_t = uint32_t;

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

/* Enum określający kierunek. */
enum Direction {
	Up = 0,
	Right = 1,
	Down = 2,
	Left = 3,
};

/* Enum określający stan rozgrywki. */
enum GameState {
	LobbyState = 0,
	GameplayState = 1,
};

/* Struktura określająca pozycję. */
class Position {
public:
	uint16_t x;
	uint16_t y;

	Position() : x(0), y(0) {}

	Position(uint16_t x, uint16_t y) : x(x), y(y) {}
};

/* Klasa określająca bombę. */
class Bomb {
public:
	Position position;
	uint16_t timer;

	Bomb(Position position, uint16_t timer)
			: position(position),
			  timer(timer) {}

	void decrease_timer() {
		timer--;
	}
};

/* Klasa określająca gracza. */
class Player {
	bool dead{false};
public:
	std::string name;
	std::string address;

	[[nodiscard]] bool is_dead() const { return dead; }

	void explode() { dead = true; }

	void revive() { dead = false; }
};

/* Rodzaj wydarzenia. */
enum EventType {
	BombPlaced = 0,
	BombExploded = 1,
	PlayerMoved = 2,
	BlockPlaced = 3,
};

/* Wydarzenie BombPlaced. */
struct BombPlaced {
	uint32_t bomb_id;
	Position position;
};

/* Wydarzenie BombExploded. */
struct BombExploded {
	uint32_t bomb_id;
	std::vector<uint8_t> robots_destroyed;
	std::vector<Position> blocks_destroyed;
};

/* Wydarzenie PlayerMoved. */
struct PlayerMoved {
	uint8_t player_id;
	Position position;
};

/* Wydarzenie BlockPlaced. */
struct BlockPlaced {
	Position position;
};

/* Klasa określająca wydarzenie. */
class Event {
public:
	EventType type;
	std::variant<struct BombPlaced, struct BombExploded,
			struct PlayerMoved, struct BlockPlaced> data;

	Event(EventType type,
	      std::variant<struct BombPlaced,
			      struct BombExploded,
			      struct PlayerMoved,
			      struct BlockPlaced> &data)
			: type(type),
			  data(data) {}
};

#endif //ZADANIE02_EVENT_H

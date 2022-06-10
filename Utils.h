#ifndef ZADANIE02_EVENT_H
#define ZADANIE02_EVENT_H

#include <cstdint>
#include <string>
#include <list>
#include <csignal>
#include <utility>
#include <variant>
#include <vector>
#include <optional>
#include <chrono>
#include <random>

#define RANDOM_BASE 2147483647
#define RANDOM_MULTIPLIER 48271

using turn_id_t = uint16_t;
using bomb_id_t = uint32_t;
using player_id_t = uint8_t;
using score_t = uint32_t;

class RandomGenerator {
	uint32_t seed;
	std::minstd_rand random;
public:
	explicit RandomGenerator(std::optional<uint32_t> seed_option) {
		if (seed_option.has_value()) {
			seed = seed_option.value();
		} else {
			seed = (uint32_t) std::chrono::system_clock::now()
					.time_since_epoch().count();
		}

		random.seed(seed);
	}

	uint64_t generate() {
		return random();
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

	void change(Position position) {
		x = position.x;
		y = position.y;
	}

	bool operator==(const Position &rhs) const {
		return x == rhs.x &&
		       y == rhs.y;
	}

	bool operator!=(const Position &rhs) const {
		return !(rhs == *this);
	}
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

	bool ready_to_explode() { return timer == 0; }
};

/* Klasa określająca gracza. */
class Player {
	bool dead{false};
public:
	std::string name;
	std::string address;

	Player(std::string &name, std::string address)
			: name(name),
			  address(std::move(address)) {}

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

	BombPlaced() : bomb_id(0), position() {}

	BombPlaced(uint32_t bomb_id, Position position)
			: bomb_id(bomb_id),
			  position(position) {}
};

/* Wydarzenie BombExploded. */
struct BombExploded {
	bomb_id_t bomb_id;
	std::vector<Position> blocks_destroyed;
	std::vector<player_id_t> robots_destroyed;

	BombExploded() : bomb_id(0) {}

	BombExploded(bomb_id_t bomb_id,
	             std::vector<Position> &blocks_destroyed,
	             std::vector<player_id_t> &robots_destroyed)
			: bomb_id(bomb_id),
			  blocks_destroyed(blocks_destroyed),
			  robots_destroyed(robots_destroyed) {}

};

/* Wydarzenie PlayerMoved. */
struct PlayerMoved {
	uint8_t player_id;
	Position position;

	PlayerMoved() : player_id(0), position() {}

	PlayerMoved(uint8_t player_id, Position position)
			: player_id(player_id),
			  position(position) {}
};

/* Wydarzenie BlockPlaced. */
struct BlockPlaced {
	Position position;

	BlockPlaced() : position() {}

	explicit BlockPlaced(Position position) : position(position) {}
};

/* Klasa określająca wydarzenie. */
class Event {
public:
	EventType type;
	std::variant<struct BombPlaced, struct BombExploded,
			struct PlayerMoved, struct BlockPlaced> data;

	Event(EventType type, struct BombPlaced &bomb_placed)
			: type(type) {
		data.emplace<struct BombPlaced>(bomb_placed);
	}

	Event(EventType type, struct BombExploded &bomb_exploded)
			: type(type) {
		data.emplace<struct BombExploded>(bomb_exploded);
	}

	Event(EventType type, struct PlayerMoved &player_moved)
			: type(type) {
		data.emplace<struct PlayerMoved>(player_moved);
	}

	Event(EventType type, struct BlockPlaced &block_placed)
			: type(type) {
		data.emplace<struct BlockPlaced>(block_placed);
	}
};

#endif //ZADANIE02_EVENT_H

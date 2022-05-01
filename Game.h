#ifndef ZADANIE02_GAME_H
#define ZADANIE02_GAME_H

#include "Utils.h"
#include "Parameters.h"
#include <map>
#include <set>
#include <vector>

using player_id_t = uint8_t;

struct Player {
	std::string name;
	std::string address;


	void join_game(player_id_t new_id) {
		score = 0;
		id = new_id;
	}

//część przydatna w stanie gameplay
private:
	player_id_t id;
	uint32_t score;
	Position position;
	bool currently_dead;

public:
	void set_position(Position new_position) { position = new_position; }

	void commit_suicide() { currently_dead = true; }

	void revive(Position new_position) {
		currently_dead = false;
		set_position(new_position);
	}

	bool is_alive() { return !currently_dead; }

//	bool operator<(const Player &rhs) const {
//		return id < rhs.id;
//	}


};

struct Field {
	bool is_block{false};
public:
	explicit Field() = default;

	void become_solid() { is_block = true; }

	void become_air() { is_block = false; }

};

class Board {
public:
	explicit Board(Parameters &parameters,
	               std::map<player_id_t, Player> &currently_playing)
			: parameters(parameters),
			  currently_playing(currently_playing),
			  random(Random(parameters.seed)) {

		fields.resize(parameters.size_x);
		for (auto &column: fields) {
			column.resize(parameters.size_y);
		}
	}

	void
	initialize() {
		for (auto &player: currently_playing) {
			uint16_t x = random.generate() % parameters.size_x;
			uint16_t y = random.generate() % parameters.size_y;
			//todo dodaj zdarzenie PlayerMoved
			player.second.set_position({x, y});
		}
		for (int i = 0; i < parameters.initial_blocks; i++) {
			uint16_t x = random.generate() % parameters.size_x;
			uint16_t y = random.generate() % parameters.size_y;
			fields[x][y].become_solid();
			//todo dodaj zdarzenie BlockPlaced
		}
	}

	void decrement_bomb_timer() {
		for (auto &bomb: bombs) {
			bomb.second.tic();
		}
	}

	void explode(Bomb &bomb) {

	}

	void activate_bombs() {
		std::vector<Bomb> bombs_to_remove;
		for (auto &bomb: bombs) {
			if (bomb.second.will_explode()) {
				explode(bomb.second);
				//todo dodaj zdarzenie BombExploded
				bombs_to_remove.push_back(bomb.second);
			}
		}
		for (auto bomb: bombs_to_remove) { //może tutaj referencja
			bombs.erase(bomb.get_id());
		}
	}

	void activate_robots() {
		for (auto &element: currently_playing) {
			Player &player = element.second;
			if (player.is_alive()) {
				//todo instrukcja robota
				// jeśli ruch legalny, dodaj zdarzenie

			} else {
				uint16_t x = random.generate() % parameters.size_x;
				uint16_t y = random.generate() % parameters.size_y;
				player.revive({x, y});
				//todo zdarzenie 'PlayerMoved'
			}
		}
	}

private:
	Random random;
	Parameters &parameters;
	std::vector<std::vector<Field>> fields;
	std::map<bomb_id_t, Bomb> bombs;
	std::map<player_id_t, Player> &currently_playing;
};

class Game {
	enum game_state {
		lobby_state,
		gameplay_state,
	};


public:
	explicit Game(Parameters &parameters)
			: parameters(parameters),
			  state(lobby_state),
			  current_round(0),
			  current_player_id(0),
			  board(Board(parameters, currently_playing)) {}

	void accept_player(Player &player) {
		player_id_t id = new_id();
		player.join_game(id);
		currently_playing.insert({id, player});
	}

	bool enaugh_players() {
		return currently_playing.size() == parameters.players_count;
	}

	player_id_t new_id() {
		return current_player_id++;
	}

	void start_game() {
		state = gameplay_state;
		current_round = 0;
		board.initialize();
		//todo po tej funkcji serwer wysyła 'Turn'
	}

	void generate_turn() {
		board.decrement_bomb_timer();
		board.activate_bombs();
		board.activate_robots();
		current_round++;
	}

	void end_game() {
		state = lobby_state;
		currently_playing.clear();
	}

private:
	Board board;
	std::map<player_id_t, Player> currently_playing;
	Parameters parameters;
	game_state state;
	int current_round;
	player_id_t current_player_id;
	//	std::list<Event> events;
	//	std::set<Bomb> bombs;
	//	std::set<Position> blocks;
};


#endif //ZADANIE02_GAME_H

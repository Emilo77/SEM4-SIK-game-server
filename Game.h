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
		currently_dead = false;
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

	bool is_on_exploded_field(Position &exploded) {
		return position == exploded;
	}
};

struct Field {
private:
	bool is_block{false};
public:
	explicit Field() = default;

	bool is_solid() { return is_block; }

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

	bool is_valid(Position &position) {
		return position.x >= 0 && position.x < parameters.size_x &&
		       position.y >= 0 && position.y < parameters.size_y;
	}

	Position random_position() {
		uint16_t x = random.generate() % parameters.size_x;
		uint16_t y = random.generate() % parameters.size_y;
		return {x, y};
	}

	void
	initialize() {
		for (auto &player: currently_playing) {
			player.second.set_position(random_position());
			//todo dodaj zdarzenie PlayerMoved
		}
		for (int i = 0; i < parameters.initial_blocks; i++) {
			Position new_position = random_position();
			fields[new_position.x][new_position.y].become_solid();
			//todo dodaj zdarzenie BlockPlaced
		}
	}

	void decrement_bomb_timer() {
		for (auto &bomb: bombs) {
			bomb.second.tic();
		}
	}

	void explode_direction(Bomb &bomb, Direction direction,
	                       std::vector<Position>
	                       &positions_to_explode) {
		Position bomb_position = bomb.get_position();
		uint16_t radius = parameters.explosion_radius;
		int horizontal_shift = 0;
		int vertical_shift = 0;

		switch (direction) {
			case Up:
				vertical_shift = 1;
				break;
			case Right:
				horizontal_shift = 1;
				break;
			case Down:
				vertical_shift = -1;
				break;
			case Left:
				horizontal_shift = -1;
				break;
		}

		for (int i = 1; i <= radius; i++) {
			Position pos(bomb_position.x + i * horizontal_shift,
			             bomb_position.y + i * vertical_shift);
			if (is_valid(pos)) {
				positions_to_explode.push_back(pos);
				if (fields[pos.x][pos.y].is_solid()) {
					break;
				}
				// break;
			}
		}
	}

	void explode(Bomb &bomb) {
		std::vector<Position> positions_to_explode;
		Position bomb_position = bomb.get_position();

		positions_to_explode.push_back(bomb_position);
		if (!fields[bomb_position.x][bomb_position.y].is_solid()) {
			// jeżeli bomba wybucha na solidnym bloku, to rozsadza tylko jego
			// (może trzeba będzie to zmienić, dopytać się)
			explode_direction(bomb, Up, positions_to_explode);
			explode_direction(bomb, Right, positions_to_explode);
			explode_direction(bomb, Down, positions_to_explode);
			explode_direction(bomb, Left, positions_to_explode);
		}

		for(auto &exploded_position : positions_to_explode) {
			fields[exploded_position.x][exploded_position.y].become_air();
			for (auto &element: currently_playing) {
				Player &player = element.second;
				if (player.is_on_exploded_field(exploded_position)) {
					player.commit_suicide();
				}
			}
		}
	}

	void activate_bombs() {
		std::vector<Bomb> bombs_to_remove;
		for (auto &element: bombs) {
			Bomb &bomb = element.second;
			if (bomb.will_explode()) {
				explode(bomb);
				//todo dodaj zdarzenie BombExploded
				bombs_to_remove.push_back(bomb);
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
				player.revive(random_position());
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
};


#endif //ZADANIE02_GAME_H

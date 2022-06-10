#ifndef ZADANIE02_GAME_H
#define ZADANIE02_GAME_H

#include "Utils.h"
#include "ServerParameters.h"
#include "Messages.h"
#include <map>
#include <set>
#include <vector>

/* Pomocnicza klasa do generowania identyfikatorów. */
class IdGenerator {
private:
	turn_id_t turn_id{0};
	bomb_id_t bomb_id{0};
	player_id_t player_id{0};

public:

	void reset() {
		turn_id = 0;
		bomb_id = 0;
		player_id = 0;
	}

	[[nodiscard]] turn_id_t last_turn_id() const { return turn_id - 1; }

	turn_id_t new_turn_id() { return turn_id++; }

	bomb_id_t new_bomb_id() { return bomb_id++; }

	player_id_t new_player_id() { return player_id++; }
};

/* Pomocnicza klasa przechowująca informacje o danym polu na planszy. */
class Field {
private:
	bool solid{false};
	bool will_be_solid{false};
	bool exploded{false};
	bool bomb_placed{false};

public:
	/* Zaznaczenie, że pole jest blokiem. */
	void make_solid() { solid = true; }

	/* Zaznaczenie, że pole jest powietrzem (nie jest blokiem). */
	void make_air() { solid = false; }

	/* Zaznaczenie, że na polu postawiono bombę w tej turze. */
	void place_bomb() { bomb_placed = true; }

	/* Zaznaczenie, że pole eksplodowało w tej turze. */
	void mark_exploded() { exploded = true; }

	/* Zaznaczenie, że pole zostanie w tej turze postawione bombą. */
	void mark_placed() { will_be_solid = true; }

	/* Zresetowanie eksplozji. */
	void reset_exploded() { exploded = false; }

	/* Zresetowanie postawienia bomby. */
	void reset_bomb_placed() { bomb_placed = false; }

	/* Zresetowanie zaznaczenia, że pole będzie blokiem. */
	void reset_will_be_solid() { will_be_solid = false; }

	/* Sprawdzenie, czy pole jest blokiem. */
	[[nodiscard]] bool is_solid() const { return solid; }

	/* Sprawdzenie, czy pole eksplodowało w aktualnej turze. */
	[[nodiscard]] bool is_exploded() const { return exploded; }

	/* Sprawdzenie, czy na polu została postawiona bomba w tej turze. */
	[[nodiscard]] bool is_bomb_placed() const { return bomb_placed; }

	/* Sprawdzenie, czy pole będzie blokiem w następnej turze. */
	[[nodiscard]] bool will_be_placed() const { return will_be_solid; }
};

/* Pomocnicza klasa planszy, dzięki niej jesteśmy w stanie szybko
 * sprawdzić informacje dotyczące danego pola. */
class Board {
	std::vector<std::vector<Field>> fields;

public:

	Field &at(Position position) {
		return fields[position.x][position.y];
	}

	/* Ustawiamy planszę na odpowiedni rozmiar i czyścimy ją. */
	void reset(uint16_t size_x, uint16_t size_y);

	/* Aktualizacja stanu pól po eksplozjach w turze. */
	void apply_explosions();

	/* Aktualizacja stanu pól po stawianiu bloków w turze. */
	void apply_blocks();

};

class Game {
public:
	explicit Game(ServerParameters &server_parameters, RandomGenerator &random)
			: random(random),
			  server_name(server_parameters.server_name),
			  players_count(server_parameters.players_count),
			  size_x(server_parameters.size_x),
			  size_y(server_parameters.size_y),
			  game_length(server_parameters.game_length),
			  explosion_radius(server_parameters.explosion_radius),
			  bomb_timer(server_parameters.bomb_timer),
			  initial_blocks(server_parameters.initial_blocks) {}

private:
	RandomGenerator random;
	enum GameState game_state{LobbyState};

	std::string server_name{};
	uint8_t players_count;
	uint16_t size_x;
	uint16_t size_y;
	uint16_t game_length;
	uint16_t explosion_radius;
	uint16_t bomb_timer;
	uint16_t initial_blocks;

	std::vector<struct Turn> turns;

	Board board;
	std::map<player_id_t, Player> players;
	std::map<player_id_t, Position> player_positions;
	std::map<player_id_t, score_t> scores;
	std::map<bomb_id_t, Bomb> bombs;

	IdGenerator id_generator;

public:
	/* Sprawdzenie, w jakim stanie jest gra. */
	bool is_gameplay();

	bool enough_players() { return players.size() >= players_count; }

	player_id_t accept_player(Player &player);

	void start_gameplay();

	void simulate_turn();

	/* Generowanie wiadomości Hello dla połączonego klienta */
	struct Hello generate_Hello();

	/* Generowanie wiadomości GameStarted dla wszystkich klientów. */
	struct GameStarted generate_GameStarted();

	/* Generowanie wiadomości Turn dla wszystkich klientów. */
	struct Turn generate_Turn(uint16_t number);

	/* Generowanie wiadomości Turn dla wszystkich klientów. */
	struct Turn generate_last_Turn();

	/* Generowanie wiadomości GameEnded dla wszystkich klientów. */
	struct GameEnded generate_GameEnded();

private:

	Position random_position();

	/* Sprawdzenie, czy pozycja mieści się w zakresie planszy. */
	[[nodiscard]] bool is_correct_position(Position position) const;

	/* Czyszczenie kontenerów z informacjami. */
	void clear_containers();

	/* Dopasowanie wielkości kontenerów na podstawie graczy. */
	void initialize_containers();

	void
	kill_players_at(Position position, std::vector<player_id_t> &destroyed);

	/* Zapisanie eksplozji pojedynczej bomby. */
	void mark_explosions(bomb_id_t id,
	                     std::vector<Position> &blocks_destroyed,
	                     std::vector<player_id_t> &players_destroyed);

	/* Zapisanie eksplozji pojedynczej bomby w danym kierunku. */
	void mark_explosions_in_direction(Position bomb_pos, Direction direction,
	                                  std::vector<Position>
	                                  &blocks_destroyed,
	                                  std::vector<player_id_t> &players_destroyed);

	/* Aktualizacja wyników graczy. */
	void change_scores_and_revive_players();

	/* Aktualizacja stanu gry na podstawie zdarzenia BombPlaced. */
	std::optional<Event>
	apply_BombPlaced(player_id_t player_id);

	/* Aktualizacja stanu gry na podstawie zdarzenia PlayerMoved. */
	std::optional<Event>
	apply_PlayerMoved(player_id_t player_id, Direction direction);

	/* Aktualizacja stanu gry na podstawie zdarzenia BlockPlaced. */
	std::optional<Event>
	apply_BlockPlaced(player_id_t player_id);

	/* Zmiana stanu gry. */
	std::optional<Event>
	apply_client_message(ClientMessage &message);
};


#endif //ZADANIE02_GAME_H

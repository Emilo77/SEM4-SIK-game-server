#include "Game.h"

void Board::reset(uint16_t size_x, uint16_t size_y) {
	/* Zmieniamy rozmiar planszy. */
	fields.resize(size_x);
	for (auto &row: fields) {
		row.resize(size_y);
	}

	/* Resetujemy wszystkie pola. */
	for (auto &column: fields) {
		for (auto &field: column) {
			field.make_air();
			field.reset_exploded();
			field.reset_bomb_placed();
			field.reset_will_be_solid();
		}
	}
}

void Board::apply_explosions() {
	/* Iterujemy się po całej planszy. */
	for (auto &field: fields) {
		for (auto &row: field) {
			/* Jeżeli pole eksplodowało w aktualnej turze, dodajemy jego
			 * pozycję do listy, zmieniamy pole na nie-blok
			 * oraz resetujemy stan eksplozji. */
			if (row.is_exploded()) {
				row.make_air();
				row.reset_exploded();
			}
		}
	}
}

void Board::apply_blocks() {
	/* Iterujemy się po całej planszy. */
	for (auto &field: fields) {
		for (auto &row: field) {
			/* Jeżeli pole eksplodowało w aktualnej turze, dodajemy jego
			 * pozycję do listy, zmieniamy pole na nie-blok
			 * oraz resetujemy stan eksplozji. */
			if (row.will_be_solid()) {
				row.make_block();
				row.reset_will_be_solid();
			}
		}
	}
}

/* Funkcja pomocnicza do obliczania eksplozji po wybuchu bomby. */
static inline std::pair<int, int> direction_to_pair(Direction direction) {
	switch (direction) {
		case Up:
			return {0, 1};
		case Right:
			return {1, 0};
		case Down:
			return {0, -1};
		case Left:
			return {-1, 0};
	}
	return {0, 0};
}

Position Game::random_position() {
	uint16_t new_x = (uint16_t) random.generate() % size_x;
	uint16_t new_y = (uint16_t) random.generate() % size_y;

	return {new_x, new_y};
}

/* Funkcja sprawdzająca, czy pozycja mieści się w zakresie planszy. */
bool Game::is_correct_position(Position position) const {
	return position.x < size_x && position.y < size_y;
}

void Game::clear_containers() {
	id_generator.reset();
	players.clear();
	player_positions.clear();
	bombs.clear();
	scores.clear();
}

void Game::initialize_containers() {
	for (auto &new_player_pair: players) {
		scores.insert({new_player_pair.first, 0});
		player_positions.insert({new_player_pair.first, Position()});
	}
}

void Game::kill_players_at(Position position, std::vector<player_id_t>
&destroyed) {
	for (auto &element: player_positions) {
		if (element.second == position) {
			destroyed.push_back(element.first);
			players.at(element.first).explode();
		}
	}
}

void Game::change_scores_and_revive_players() {
	for (auto &player_pair: players) {
		if (player_pair.second.is_dead()) {
			scores.at(player_pair.first)++;
			player_pair.second.revive();
		}
	}
}

void Game::mark_explosions(bomb_id_t id, std::vector<Position>
&blocks_destroyed, std::vector<player_id_t> &players_destroyed) {

	Position bomb_position = bombs.at(id).position;

	/* Oznaczamy pole na pozycji bomby. */
	board.at(bomb_position).mark_exploded();

	/* Jeżeli bomba eksplodowała na bloku, kończymy działanie,
	 * bomba rozsadza tylko pojedynczy blok, na którym została położona. */
	if (board.at(bomb_position).is_solid()) {
		blocks_destroyed.push_back(bomb_position);
		kill_players_at(bomb_position, players_destroyed);
		return;
	}

	/* W przeciwnym wypadku obliczamy eksplozje w czterech kierunkach od bomby.*/
	mark_explosions_in_direction(bomb_position, Up, blocks_destroyed,
	                             players_destroyed);
	mark_explosions_in_direction(bomb_position, Right, blocks_destroyed,
	                             players_destroyed);
	mark_explosions_in_direction(bomb_position, Down, blocks_destroyed,
	                             players_destroyed);
	mark_explosions_in_direction(bomb_position, Left, blocks_destroyed,
	                             players_destroyed);
}

void
Game::mark_explosions_in_direction(Position bomb_pos, Direction direction,
                                   std::vector<Position> &blocks_destroyed,
                                   std::vector<player_id_t> &players_destroyed) {
	auto pair = direction_to_pair(direction);

	for (int i = 1; i <= explosion_radius; i++) {
		int new_x = bomb_pos.x + i * pair.first;
		int new_y = bomb_pos.y + i * pair.second;

		/* Otrzymujemy nowe pozycje w określonym kierunku od bomby. */
		Position new_position(static_cast<uint16_t>(new_x),
		                      static_cast<uint16_t>(new_y));


		/* Sprawdzamy, czy eksplozja mieści się w zakresie planszy. */
		if (is_correct_position(new_position)) {
			/* Oznaczamy pole, dodajemy zniszczonych przeciwników */
			board.at(new_position).mark_exploded();
			kill_players_at(new_position, players_destroyed);
			/* Jeżeli eksplodował blok, wychodzimy z pętli,
			 * ponieważ eksplozje nie docierają za blokami. */
			if (board.at(new_position).is_solid()) {
				blocks_destroyed.push_back(new_position);
				break;
			}
		}
	}
}

bool Game::is_gameplay() {
	return game_state == GameplayState;
}

void Game::accept_player(Player &player) {
//	players.insert({player.get_id(), player});
}

void Game::start_gameplay() {
	/* Zmieniamy stan gry na Gameplay. */
	game_state = GameplayState;

	/* Nadajemy graczom domyślne pozycje i wyniki. */
	initialize_containers();

	/* Inicjujemy kontener na zdarzenia. */
	std::vector<Event> new_events;

	for (auto &player: players) {

		/* Losujemy pozycję i ustawiamy tam gracza. */
		Position new_position = random_position();
		player_positions.at(player.first).change(new_position);

		/* Dodajemy zdarzenie PlayerMoved do kontenera zdarzeń. */
		struct PlayerMoved data(player.first, new_position);
		new_events.emplace_back(PlayerMoved, data);
	}

	for (int i = 0; i < initial_blocks; i++) {

		/* Losujemy pozycję do postawienia bloku. */
		Position new_position = random_position();
		board.at(new_position).make_block();

		/* Dodajemy zdarzenie BlockPlaced do kontenera zdarzeń. */
		struct BlockPlaced data(new_position);
		new_events.emplace_back(BlockPlaced, data);
	}

	/* Dodajemy turę do kontenera tur. */
	turns.emplace_back(id_generator.new_turn_id(), new_events);
}

void Game::simulate_turn() {

	std::vector<Event> new_events;
	std::vector<bomb_id_t> bombs_to_remove;

	for (auto &bomb: bombs) {
		/* Zmniejszamy timer bomby. */
		bomb.second.decrease_timer();
		/* Jeżeli bomba powinna eksplodować, symulujemy eksplozję. */
		if (bomb.second.ready_to_explode()) {

			/* Inicjujemy kontenery dla zniszczonych graczy i bloków. */
			std::vector<player_id_t> players_exploded;
			std::vector<Position> blocks_exploded;

			/* Wyznaczamy zniszczonych graczy i zniszczone bloki. */
			mark_explosions(bomb.first, blocks_exploded, players_exploded);

			/* Dodajemy zdarzenie BombExploded do kontenera zdarzeń. */
			struct BombExploded data(bomb.first, players_exploded,
			                         blocks_exploded);
			new_events.emplace_back(BombExploded, data);

			/* Zaznaczamy, że należy usunąć bombę. */
			bombs_to_remove.push_back(bomb.first);
		}
	}

	/* Usuwamy wszystkie bomby, które eksplodowały w tej turze. */
	for (auto &id: bombs_to_remove) {
		bombs.erase(id);
	}

	/* Usuwamy wszystkie bloki, które zostały zniszczone przez eksplozje. */
	board.apply_explosions();

	/* Obsługujemy graczy. */
	for (auto &player: players) {
		/* Jeżeli gracz */
		if (!player.second.is_dead()) {
			//obsłuż ruch gracza

		/* Jeżeli gracz eksplodował w tej turze,
		 * przenosimy go na losową pozycję i ignorujemy jego ruch. */
		} else {
			Position new_position = random_position();
			player_positions.at(player.first) = new_position;

			struct PlayerMoved data(player.first, new_position);
			new_events.emplace_back(PlayerMoved, data);
		}
	}

	/* Zwiększamy wynik wszystkim graczom, którzy eksplodowali
	 * oraz "przywracamy ich do życia". */
	change_scores_and_revive_players();

	/* Wstawiamy wszystkie bloki postawione w tej turze przez graczy. */
	board.apply_blocks();

	/* Dodajemy turę to kontenera tur. */
	turns.emplace_back(id_generator.new_turn_id(), new_events);
}

struct Hello Game::generate_Hello() {
	return {server_name, players_count, size_x, size_y, game_length,
	        explosion_radius, bomb_timer};
}

struct GameStarted Game::generate_GameStarted() {
	return {players};
}

struct Turn Game::generate_Turn() {

}

std::optional<Event> Game::apply_Join(/* ???*/) {

	std::optional<Event> new_event;

}

std::optional<Event> Game::apply_BombPlaced(player_id_t player_id) {

	std::optional<Event> new_event;
	/* Jeżeli nie istnieje gracz o podanym id lub gracz eksplodował w tej
	* turze, nic nie robimy. */
	if (players.find(player_id) == players.end() ||
	    players.at(player_id).is_dead()) {
		return {};
	}

	/* Wyciągamy pozycję gracza. */
	Position player_pos = player_positions.at(player_id);

	/* Sprawdzamy, czy w tej turze, na tym polu, nie została postawiona już
	 * bomba.*/
	if (!board.at(player_pos).is_bomb_placed()) {

		/* Oznaczamy, że na polu postawiono bombę. */
		board.at(player_pos).place_bomb();

		/* Generujemy nowe id */
		bomb_id_t new_bomb_id = id_generator.new_bomb_id();

		/* Dodajemy bombę do mapy bomb. */
		bombs.insert({new_bomb_id, Bomb(player_pos, bomb_timer)});

		/* Dodajemy zdarzenie BombPlaced do listy zdarzeń w tej turze. */
		struct BombPlaced data(new_bomb_id, player_pos);
		new_event.emplace(EventType::BombPlaced, data);

		return new_event;
	}

}

std::optional<Event>
Game::apply_PlayerMoved(player_id_t player_id, Direction direction) {

	std::optional<Event> new_event;

	/* Jeżeli nie istnieje gracz o podanym id lub gracz eksplodował w tej
	 * turze, nic nie robimy. */
	if (players.find(player_id) == players.end() ||
	    players.at(player_id).is_dead()) {
		return {};
	}

	std::pair<int, int> pair = direction_to_pair(direction);

	/* Tworzymy współrzędne na podstawie otrzymanego Direction */
	int pos_x = player_positions.at(player_id).x + pair.first;
	int pos_y = player_positions.at(player_id).y + pair.second;

	/* Tworzymy nową, hipotetyczną pozycję gracza. */
	Position new_position(static_cast<uint16_t>(pos_x),
	                      static_cast<uint16_t>(pos_y));

	/* Sprawdzamy:
	 * czy nowa pozycja gracza jest prawidłowa,
	 * czy na nowej pozycji można stanąć (nie znajduje się tam blok). */
	if (is_correct_position(new_position)
	    && !board.at(new_position).is_solid()) {

		/* Zmieniamy położenie gracza. */
		player_positions.at(player_id) = new_position;

		/* Jeżeli ruch jest dozwolony, dodajemy zdarzenie PlayerMoved
		 * do listy zdarzeń w tej turze. */
		struct PlayerMoved data(player_id, new_position);
		new_event.emplace(EventType::PlayerMoved, data);

		return new_event;
	}
}

std::optional<Event> Game::apply_BlockPlaced(player_id_t player_id) {

	std::optional<Event> new_event;
	/* Jeżeli nie istnieje gracz o podanym id lub gracz eksplodował w tej
    * turze, nic nie robimy. */
	if (players.find(player_id) == players.end() ||
	    players.at(player_id).is_dead()) {
		return {};
	}

	Position block_position = player_positions.at(player_id);

	/* Sprawdzamy, czy pole nie jest już blokiem i czy w tej turze ktoś nie
	 * postawił już na nim bloku. */
	if (!board.at(block_position).is_solid()
	    && !board.at(block_position).will_be_solid()) {

		/* Oznaczamy pole, że zamieni się w blok w następnej turze. */
		board.at(block_position).mark_placed();

		/* Dodajemy zdarzenie BlockPlaced do listy zdarzeń. */
		struct BlockPlaced data(block_position);
		new_event.emplace(EventType::BlockPlaced, data);

		return new_event;
	}
	return {};
}

std::optional<Event> Game::apply_client_message(ClientMessage &message) {
	if (game_state == LobbyState) {
		if (message.type == Join) {
			return apply_Join();
		}
	} else {
		switch (message.type) {
			case Join:
				break;
			case PlaceBomb:
				return apply_BombPlaced(message.player_id);
			case PlaceBlock:
				return apply_BlockPlaced(message.player_id);
			case Move:
				return apply_PlayerMoved(message.player_id,
				                         std::get<Direction>(message.data));
		}
	}
	return {};
}
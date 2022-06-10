#include "GameRoom.h"
#include "Connection.h"

void GameRoom::connect_to_game_room(const gamer_ptr &gamer) {
	gamers_.insert(gamer);
	auto message = ServerMessage(Hello, game_info.generate_Hello());

	/* Wysyłamy Hello do danego gracza. */
	gamer->deliver(message);

	if (game_info.is_gameplay()) {
		/* Jeżeli gra się rozpoczęła, wysyłamy komunikatGameStarted
		 * oraz wszystkie dotychczasowe tury. */
		auto game_started =
				ServerMessage(GameStarted, game_info.generate_GameStarted());
		gamer->deliver(game_started);

		/* Wysyłamy wszystkie dotychczasowe tury. */
		send_all_turns(gamer);
	} else {
		/* Jeżeli gra się nie rozpoczęła, wysyłamy wszystkie dotychczasowe
		 * wiadomości AcceptedPlayer. */
		send_all_accepted_players(gamer);
	}
}

void GameRoom::send_all_turns(const gamer_ptr &gamer) {
	size_t size = game_info.get_turns_size();
	for (size_t i = 0; i < size; i++) {
		auto turn = ServerMessage(Turn, game_info.generate_Turn(i));
		gamer->deliver(turn);
	}
}

void GameRoom::send_all_accepted_players(const gamer_ptr &gamer) {
	for (auto &message: accepted_players_messages) {
		gamer->deliver(message);
	}
}


void GameRoom::leave(const gamer_ptr &gamer) {
	gamers_.erase(gamer);
	if (gamer->get_id().has_value()) {
		player_id_t id = gamer->get_id().value();
		if (last_messages.find(id) != last_messages.end()) {
			last_messages.erase(id);
		}
	}
}

void GameRoom::accept_to_game(const gamer_ptr &gamer, ClientMessage &message) {
	Player new_player(std::get<std::string>(message.data),
	                  gamer->get_name());

	/* Przekazujemy zaakceptowanego gracza do stanu gry. */
	player_id_t new_id = game_info.accept_player(new_player);

	/* Ustawiamy naszemu klientowi otrzymane id. */
	gamer->set_id(new_id);

	/* Tworzymy wiadomość AcceptedPlayer */
	struct AcceptedPlayer data(new_id, new_player);
	auto accepted_player = ServerMessage(AcceptedPlayer, data);

	/* Dodajemy wiadomość AcceptedPlayer do kontenera,
	 * aby móc ją ewentualnie wysłać do następnych graczy. */
	accepted_players_messages.push_back(accepted_player);

	/* Wysyłamy informację, że gracz został zaakceptowany. */
	send_to_all(accepted_player);

	/* Jeżeli liczba graczy jest wystarczająca, rozpoczynamy grę. */
	if (game_info.enough_players()) {
		handle_game();
	}
}


void GameRoom::get_message(const gamer_ptr &gamer, ClientMessage &message) {
	if (game_info.is_gameplay()) {
		/* Jeśli trwa rozgrywka i klient jest graczem,
		 * dodajemy jego wiadomość. */
		if (gamer->get_id().has_value()) {
			last_messages.insert_or_assign(gamer->get_id().value(), message);
		}
	} else {
		if (message.type == Join && !gamer->get_id().has_value()) {
			/* Jeżeli klient wysyła Join i nie jest zaakceptowany,
			 * dodajemy go do zbioru graczy. */
			accept_to_game(gamer, message);
		}
	}
}

void GameRoom::send_to_all(ServerMessage &message) {
	for (auto &gamer: gamers_) {
		gamer->deliver(message);
	}
}

void GameRoom::remove_all_ids() {
	for (auto &gamer: gamers_) {
		gamer->remove_id();
	}
}

void GameRoom::simulate_turns() {
	timer.expires_from_now(
			boost::asio::chrono::milliseconds(parameters.turn_duration));

	timer.async_wait([this](const boost::system::error_code &error) {

		if (!error) {
			game_info.simulate_turn(last_messages);
			last_messages.clear();
			auto turn = ServerMessage(Turn, game_info.generate_last_Turn());
			send_to_all(turn);

			if (!game_info.should_end()) {
				simulate_turns();
			} else {
				auto game_ended = ServerMessage(GameEnded,
				                                game_info.generate_GameEnded());
				send_to_all(game_ended);
				game_info.reset_all();
				remove_all_ids();
			}

		} else {
			std::cerr << "Błąd przy oczekiwaniu na ruchy klientów" << std::endl;
		}
	});
}

void GameRoom::handle_game() {
	game_info.start_gameplay();

	auto game_started = ServerMessage(GameStarted,
	                                  game_info.generate_GameStarted());

	send_to_all(game_started);

	auto turn_zero = ServerMessage(Turn, game_info.generate_last_Turn());
	send_to_all(turn_zero);

	simulate_turns();
}




#include "GameRoom.h"
#include "Connection.h"

void GameRoom::connect_to_game_room(const gamer_ptr &gamer) {
	gamers_.insert(gamer);
	auto message = ServerMessage(Hello, game_info.generate_Hello());

	/* Wysyłamy Hello do danego gracza. */
	gamer->deliver(message);
}


void GameRoom::leave(const gamer_ptr &gamer) {
	gamers_.erase(gamer);
	if (last_messages.find(gamer) != last_messages.end()) {
		last_messages.erase(gamer);
	}
}


void GameRoom::get_message(const gamer_ptr &gamer, ClientMessage &message) {
	std::cerr << "weszło do get_message" << std::endl;
	if (game_info.is_gameplay()) {
		last_messages.insert_or_assign(gamer, message);
	} else {
		if (message.type == Join) {
			Player new_player(std::get<std::string>(message.data),
			                  gamer->get_name());

			/* Przekazujemy zaakceptowanego gracza do stanu gry. */
			player_id_t new_id = game_info.accept_player(new_player);

			/* Ustawiamy naszemu klientowi otrzymane id. */
			gamer->set_id(new_id);

			/* Tworzymy wiadomość AcceptedPlayer */
			struct AcceptedPlayer data(new_id, new_player);
			auto accepted_player = ServerMessage(AcceptedPlayer, data);

			/* Wysyłamy informację, że gracz został zaakceptowany. */
			send_to_all(accepted_player);

			/* Jeżeli liczba graczy jest wystarczająca, rozpoczynamy grę. */
			if (game_info.enough_players()) {
				std::cerr << "weszło do enough" << std::endl;
				start_game();
			}
		}
	}
}

void GameRoom::send_to_all(ServerMessage &message) {
	for (auto &gamer: gamers_) {
		gamer->deliver(message);
	}
}

void GameRoom::start_game() {
	game_info.start_gameplay();
	std::cerr << "Weszło do start gameplay" << std::endl;

	auto game_started = ServerMessage(GameStarted,
	                                  game_info.generate_GameStarted());

	std::cerr << "Wygenerowało wiadomość" << std::endl;
	send_to_all(game_started);

	auto turn_zero = ServerMessage(Turn, game_info.generate_last_Turn());

	send_to_all(turn_zero);

	std::cerr << "Wysłano do wszystkich" << std::endl;

	timer.async_wait([this](const boost::system::error_code &error) {
		if (!error) {


		} else {
			std::cerr << "Błąd przy oczekiwaniu na ruchy klientów" << std::endl;
		}
	});
}




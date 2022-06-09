#include "GameRoom.h"
#include "Connection.h"

void GameRoom::connect_to_game_room(const gamer_ptr& gamer) {
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
	if(game_info.is_gameplay()) {
		last_messages.insert_or_assign(gamer, message);
	} else {
		if (message.type == Join) {
			Player new_player(std::get<std::string>(message.data),
			        gamer->get_name());

			player_id_t new_id = game_info.accept_player(new_player);
			gamer->set_id(new_id);

			if (game_info.enaugh_players()) {
				start_game();
			}
		}
	}
}

void GameRoom::start_game() {
	game_info.start_gameplay();
	timer.async_wait([this](const boost::system::error_code &error) {
		if(!error) {

		} else {
//			std::cerr <<
		}
	});
}




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
}


void GameRoom::get_message(const gamer_ptr &gamer, ClientMessage &message) {
	//todo
}


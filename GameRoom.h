#ifndef ZADANIE02_SERVER_GAMEROOM_H
#define ZADANIE02_SERVER_GAMEROOM_H

#include "ServerParameters.h"
#include "Game.h"
#include "Utils.h"


class ServerConnection;
class Connection;

typedef std::shared_ptr<ServerConnection> gamer_ptr;

class GameRoom {
public:
	GameRoom(ServerParameters &parameters, Game &game_info)
			: parameters(parameters),
			  game_info(game_info) {}

	void connect_to_game_room(gamer_ptr gamer);

	void leave(const gamer_ptr &gamer);

	void get_message(const gamer_ptr &gamer, ClientMessage &message);

private:
	std::set<gamer_ptr> gamers_;

	ServerParameters parameters;
	Game game_info;
};


#endif //ZADANIE02_SERVER_GAMEROOM_H

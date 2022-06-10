#ifndef ZADANIE02_SERVER_GAMEROOM_H
#define ZADANIE02_SERVER_GAMEROOM_H

#include <boost/asio.hpp>
#include "ServerParameters.h"
#include "Game.h"
#include "Utils.h"


class ServerConnection;
class Connection;

typedef std::shared_ptr<ServerConnection> gamer_ptr;

class GameRoom {
public:
	GameRoom(ServerParameters &parameters, Game &game_info,
	         boost::asio::steady_timer timer)
			: timer(std::move(timer)),
			  parameters(parameters),
			  game_info(game_info) {}

	void connect_to_game_room(const gamer_ptr& gamer);

	void leave(const gamer_ptr &gamer);

	void get_message(const gamer_ptr &gamer, ClientMessage &message);

	void send_to_all(ServerMessage &message);

	void start_game();

private:
	std::set<gamer_ptr> gamers_;
	std::map<gamer_ptr, ClientMessage> last_messages;
	boost::asio::steady_timer timer;

	ServerParameters parameters;
	Game game_info;
};


#endif //ZADANIE02_SERVER_GAMEROOM_H

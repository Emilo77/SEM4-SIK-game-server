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

	void connect_to_game_room(const gamer_ptr &gamer);

	void send_all_turns(const gamer_ptr &gamer);

	void send_all_accepted_players(const gamer_ptr &gamer);

	void leave(const gamer_ptr &gamer);

	void get_message(const gamer_ptr &gamer, ClientMessage &message);

	void accept_to_game(const gamer_ptr &gamer, ClientMessage &message);

	void send_to_all(ServerMessage &message);

	void remove_all_ids();

	void handle_game();

private:
	void simulate_turns();

	std::set<gamer_ptr> gamers_;
	std::map<player_id_t, ClientMessage> last_messages;
	boost::asio::steady_timer timer;
	std::vector<ServerMessage> accepted_players_messages;

	ServerParameters parameters;
	Game game_info;
};


#endif //ZADANIE02_SERVER_GAMEROOM_H

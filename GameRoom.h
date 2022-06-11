#ifndef ZADANIE02_SERVER_GAMEROOM_H
#define ZADANIE02_SERVER_GAMEROOM_H

#include <boost/asio.hpp>
#include "ServerParametersParser.h"
#include "Game.h"
#include "Utils.h"


class ServerConnection;

class Connection;

typedef std::shared_ptr<ServerConnection> gamer_ptr;

class GameRoom {
public:
	GameRoom(ServerParametersParser &parameters, Game &game_info,
	         boost::asio::steady_timer timer)
			: parameters(parameters),
			  game_info(game_info),
			  timer(std::move(timer)) {}

	/* Dodaje gracza do pokoju. */
	void connect_to_game_room(const gamer_ptr &gamer);

	/* Usuwa gracza z pokoju. */
	void leave(const gamer_ptr &gamer);

	/* Przekazuje wiadomość od klienta do pokoju. */
	void get_message(const gamer_ptr &gamer, ClientMessage &message);

private:
	/* Wysyła graczowi wszystkie dotychczasowe tury. */
	void send_all_turns(const gamer_ptr &gamer);

	/* Wysyła graczowi wszystkich połączonych graczy. */
	void send_all_accepted_players(const gamer_ptr &gamer);

	/* Oznacza klienta jako gracza w rozgrywce. */
	void accept_to_game(const gamer_ptr &gamer, ClientMessage &message);

	/* Wysyła wiadomość do wszystkich klientów. */
	void send_to_all(ServerMessage &message);

	/* Usuwa wszystkim klientom status gracza. */
	void remove_all_ids();

	/* Obsługuje początek rozgrywki. */
	void handle_game();

	/* Zarządza turami i końcem rozgrywki. */
	void simulate_turns();

	ServerParametersParser parameters;
	Game game_info;

	std::set<gamer_ptr> gamers_;
	std::map<player_id_t, ClientMessage> last_messages;
	boost::asio::steady_timer timer;
	std::vector<ServerMessage> accepted_players_messages;
};


#endif //ZADANIE02_SERVER_GAMEROOM_H

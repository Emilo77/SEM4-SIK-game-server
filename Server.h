#ifndef ZADANIE02_SERVER_H
#define ZADANIE02_SERVER_H

#include "Utils.h"
#include "ServerParametersParser.h"
#include "Buffer.h"
#include "Game.h"
#include "Connection.h"
#include "GameRoom.h"

class Server {

public:
	explicit Server(Game &game, ServerParametersParser &parameters)
			: _acceptor(_io_context, tcp::endpoint(tcp::v6(), parameters.port)),
			  _game_room(parameters, game,
			             boost::asio::steady_timer(_io_context,
			                                       boost::asio::chrono::milliseconds(
					                                       parameters.turn_duration))) {}

	void run();

private:
	void do_accept();

	boost::asio::io_context _io_context;
	tcp::acceptor _acceptor;
	GameRoom _game_room;
};


#endif //ZADANIE02_SERVER_H

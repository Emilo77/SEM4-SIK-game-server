#ifndef ZADANIE02_SERVER_H
#define ZADANIE02_SERVER_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <boost/asio.hpp>

#include "Utils.h"
#include "ServerParameters.h"
#include "Buffer.h"
#include "Game.h"
#include "Connection.h"
#include "GameRoom.h"

using boost::asio::ip::tcp;

class Server {

public:
	explicit Server(Game &game, ServerParameters &parameters) :
			_acceptor(_io_context, tcp::endpoint(tcp::v6(), parameters.port)),
			_game_room(parameters, game,
			           boost::asio::steady_timer(_io_context,
			                                     boost::asio::chrono::milliseconds(
					                                     parameters.turn_duration))) {}

	void run() {
		try {
			do_accept();
			std::cerr << "Started accepting connections ..." << std::endl;
			_io_context.run();

		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	}

private:
	void do_accept() {

		_acceptor.async_accept(
				[this](boost::system::error_code ec, tcp::socket socket) {
					if (!ec) {
						std::make_shared<Connection>(std::move(socket),
						                             _game_room)->do_start();
					}
					do_accept();
				});
	}

	boost::asio::io_context _io_context;
	tcp::acceptor _acceptor;
	GameRoom _game_room;
};


#endif //ZADANIE02_SERVER_H

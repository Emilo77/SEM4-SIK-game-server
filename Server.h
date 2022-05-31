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

using boost::asio::ip::tcp;

class Server {

public:
	explicit Server(Game &game, ServerParameters &parameters) :
			game(game),
			parameters(parameters),
			_acceptor(_io_context, tcp::endpoint(tcp::v6(), parameters.port)) {}


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

		auto connection = Connection::create(_io_context, game);
		_connections.push_back(connection);

		_acceptor.async_accept(connection->Socket(), [connection, this](const
		                                                                boost::system::error_code &error) {
			if (!error) {
				connection->start();
			}
			do_accept();
		});
	}

	Game game;
	Buffer buffer;
	ServerParameters parameters;

	boost::asio::io_context _io_context;
	tcp::acceptor _acceptor;
	std::vector<Connection::pointer> _connections;
};


#endif //ZADANIE02_SERVER_H

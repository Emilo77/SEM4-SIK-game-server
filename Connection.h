#ifndef ZADANIE02_SERVER_CONNECTION_H
#define ZADANIE02_SERVER_CONNECTION_H

#include <iostream>
#include <memory>
#include <boost/asio.hpp>

#include "Buffer.h"
#include "Game.h"

using boost::asio::ip::tcp;

class Connection : public std::enable_shared_from_this<Connection> {

public:
	using pointer = std::shared_ptr<Connection>;

	static pointer create(boost::asio::io_context &ioContext, Game &game) {
		return pointer(new Connection(ioContext, game));
	}

	tcp::socket &Socket() {
		return _socket;
	}

	void start();

private:
	explicit Connection(boost::asio::io_context &ioContext, Game &game)
			: game(game),
			  _socket(ioContext) {
		buffer.initialize(SMALL_BUFFER_SIZE);
	}

	Buffer buffer;
	Game &game;
	tcp::socket _socket;
};


#endif //ZADANIE02_SERVER_CONNECTION_H

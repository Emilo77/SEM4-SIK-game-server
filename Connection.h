#ifndef ZADANIE02_SERVER_CONNECTION_H
#define ZADANIE02_SERVER_CONNECTION_H

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "GameRoom.h"
#include "Buffer.h"

class ServerConnection {
public:
	virtual ~ServerConnection() = default;

	virtual void deliver(ServerMessage &msg) = 0;
};

using boost::asio::ip::tcp;

class Connection :
		public ServerConnection,
		public std::enable_shared_from_this<Connection> {

public:
	~Connection() override {
		if (socket_.is_open()) {
			socket_.close();
		}
	}

	explicit Connection(tcp::socket socket, GameRoom &game_room)
			: socket_(std::move(socket)),
			  game_room(game_room) {
		buffer.initialize(BUFFER_SIZE);
	}

	void do_start();

	void deliver(ServerMessage &message) override;

private:
	void do_receive();

	void handle_receive(size_t bytesTransferred);

	Buffer buffer;
	tcp::socket socket_;
	GameRoom &game_room;
};


#endif //ZADANIE02_SERVER_CONNECTION_H

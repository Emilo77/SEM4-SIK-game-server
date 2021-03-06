#ifndef ZADANIE02_SERVER_CONNECTION_H
#define ZADANIE02_SERVER_CONNECTION_H

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "GameRoom.h"
#include "Buffer.h"

#define NO_BYTES 0

/* Pomocnicza klasa abstrakcyjna do obsługi połączenia klient-serwer. */
class ServerConnection {
public:
	virtual ~ServerConnection() = default;

	virtual void deliver(ServerMessage &msg) = 0;

	virtual std::string get_name() { return "Unknown"; }

	virtual std::optional<player_id_t> get_id() { return {}; }

	virtual void set_id(player_id_t) {}

	virtual void remove_id() {}
};

using boost::asio::ip::tcp;

class Connection :
		public ServerConnection,
		public std::enable_shared_from_this<Connection> {

public:
	~Connection() override {
		/* Zamknięcie gniazda. */
		if (socket_.is_open()) {
			socket_.close();
		}
	}

	explicit Connection(tcp::socket socket, GameRoom &game_room)
			: socket_(std::move(socket)),
			  game_room(game_room) {
		/* Inicjujemy bufor na odpowiednią wielkość. */
		buffer.initialize(MAX_PACKAGE_SIZE, MID_SIZE);

		/* Zapisujemy adres ip klienta. */
		name = boost::lexical_cast<std::string>(socket_.remote_endpoint());

		/* Wyłączamy algorytm Nagle'a. */
		socket_.set_option(tcp::no_delay(true));
	}

	/* Rozpoczynanie połączenia. */
	void do_start();

	/* Wysyłanie wiadomości do klienta. */
	void deliver(ServerMessage &message) override;

	/* Pobranie nazwy klienta. */
	std::string get_name() override { return name; }

	/* Pobranie możliwego id od klienta. */
	std::optional<player_id_t> get_id() override { return player_id; }

	/* Ustawianie id klienta. */
	void set_id(player_id_t id) override { player_id.emplace(id); }

	/* Usunięcie id klienta. */
	void remove_id() override { player_id.reset(); }

private:
	/* Rozpoczęcie odbierania wiadomości. */
	void do_receive();

	/* Obsługa odebranej wiadomości. */
	void handle_receive(size_t bytesTransferred);

	Buffer buffer;
	tcp::socket socket_;

	std::optional<player_id_t> player_id;
	std::string name;
	GameRoom &game_room;
};


#endif //ZADANIE02_SERVER_CONNECTION_H

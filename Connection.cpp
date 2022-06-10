#include "Connection.h"

void Connection::do_start() {
	std::cerr << "Client " + name << " connected!" << std::endl;

	/* Powiadamiamy pokój gry, że się połączyliśmy. */
	game_room.connect_to_game_room(shared_from_this());

	/* Rozpoczynamy odbieranie wiadomości. */
	do_receive();
}

void Connection::deliver(ServerMessage &message) {
	/* Wkładamy wiadomość do bufora. */
	size_t send = buffer.insert_ServerMessage(message);
	std:: cerr << message.type << std::endl;

	buffer.print(60);

	/* Wkładamy wiadomość do bufora. */
	socket_.async_send(
			boost::asio::buffer(buffer.get_send(), send),
			[this](
					const boost::system::error_code &error,
					size_t bytesTransferred) {

				/* W przypadku błędu rozłączamy klienta. */
				if (error) {
					std::cerr << "Failed to send message!\n";
					game_room.leave(shared_from_this());

				} else {
					std::cerr << "Sent " << bytesTransferred << " bytes!\n";
				}
			});
}

void Connection::do_receive() {
	/* Asynchronicznie oczekujemy na wiadomość. */
	socket_.async_receive(boost::asio::buffer(buffer.get_receive(),
	                                          buffer.get_receive_size()),
	                      [this](
			                      const boost::system::error_code &error,
			                      size_t bytesTransferred) {
		                      /* Otrzymano poprawnie komunikat. */
		                      if (!error) {
			                      std::cerr << "Received " << bytesTransferred
			                                << "bytes!\n";

			                      /* Obsługujemy otrzymaną wiadomość. */
			                      handle_receive(bytesTransferred);

			                      /* Sytuacja, w której klient się rozłączył. */
		                      } else if (error == boost::asio::error::eof) {
			                      std::cerr << "Client disconnected properly."
			                                << std::endl;

			                      /* Opuszczamy pokój gry. */
			                      game_room.leave(shared_from_this());

			                      /* Sytuacja, w której klient rozłączył się
								   * nieprawidłowo. */
		                      } else if (error) {
			                      std::cerr
					                      << "Client disconnected incorrectly."
					                      << std::endl;

			                      /* Opuszczamy pokój gry. */
			                      game_room.leave(shared_from_this());
		                      }
	                      });
}

void Connection::handle_receive(size_t bytesTransferred) {
	try {
		/* Wyciągamy wiadomość z bufora. */
		auto message = buffer.receive_ClientMessage(bytesTransferred, player_id);

		/* Przekazujemy ją do pokoju gry. */
		game_room.get_message(shared_from_this(), message);

		handle_receive(NO_BYTES);

	} catch (IncompleteMessage &e) {
		/* W przypadku niekompletnej wiadomości oczekujemy asynchronicznie na
        * resztę. */
		do_receive();

	} catch (InvalidMessage &e) {
		/* W przypadku nieprawidłowej wiadomości rozłączamy gracza. */
		game_room.leave(shared_from_this());
	}
}
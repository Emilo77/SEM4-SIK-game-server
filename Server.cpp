#include "Server.h"

void Server::run() {
	try {
		/* Rozpoczynamy akceptowanie nowych klientów. */
		do_accept();
		std::cerr << "Started accepting connections ..." << std::endl;

		/* Uruchamiamy kontekst do asynchroniczności. */
		_io_context.run();


	} catch (std::exception &e) {
		/* W przypadku błędu podczas akceptowania
		 * kończymy działanie serwera. */
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Server::do_accept() {

		_acceptor.async_accept(
				[this](boost::system::error_code ec, tcp::socket socket) {
					if (!ec) {
						/* Jeśli klient zostanie zaakceptowany, tworzymy
						 * dla niego klasę połączenia. */
						std::make_shared<Connection>(std::move(socket),
						                             _game_room)->do_start();
					}
					do_accept();
				});
}

int main(int argc, char *argv[]) {
	ServerParametersParser parser(argc, argv);
	ServerParameters parameters = parser.check_parameters();
	Game game(parameters);
	Server server(game, parameters);
	server.run();
}

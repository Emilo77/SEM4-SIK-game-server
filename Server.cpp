#include "Server.h"

class Server {

public:
	explicit Server(Game &game, ServerParameters &parameters) :
			game(game),
			parameters(parameters) {}

	void initialize() {}

	void run() {
		while(true) {
			wait_for_players();
			game.start();
			for(int i = 0; i < parameters.game_length; i++) {
				wait_for_messages();
				game.generate_turn();
			}
			game.end();
		}
	}

private:
	Game game;
	Buffer buffer;
	ServerParameters parameters;
	size_t active_clients = 0;
};


int main(int argc, char *argv[]) {
	ServerParameters parameters(argc, argv);
	Game game(parameters);
	Server server(game, parameters);
	server.run();

	return 0;
}

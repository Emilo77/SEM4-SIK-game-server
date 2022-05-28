#include "Server.h"

class Server {

public:
	explicit Server(Game &game, ServerParameters &parameters) :
			game(game),
			parameters(parameters) {}

	void initialize() {}

	void run() {}

private:
	Game game;
	Buffer buffer;
	ServerParameters &parameters;
	size_t active_clients = 0;
};


int main(int argc, char *argv[]) {
	ServerParameters parameters(argc, argv);
	Game game(parameters);
	Server server(game, parameters);
	server.run();

	return 0;
}

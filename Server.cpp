#include "Server.h"

int main(int argc, char *argv[]) {

	ServerParameters parameters(argc, argv);
	RandomGenerator random(parameters.seed);
	Game game(parameters, random);

	Server server(game, parameters);
	server.run();
}

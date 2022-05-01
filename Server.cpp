#include "Server.h"

class Server {
private:
	Game game;

public:
	explicit Server(Parameters &p) : parameters(p), random(parameters.seed) {}

	void handle_received_message() {
		string new_player_name;
		switch ((int) buffer.get_message_id()) {
			case 0:
				new_player_name = buffer.receive_join();
				//todo parse join
				break;
			case 1:
				//todo placeBomb
				break;
			case 2:
				//todo place_block
				break;
			case 3:
				Direction direction = buffer.receive_move();
				//todo move
		}
	}

	void run() {

	}

private:
	Parameters parameters;
	Random random;
	Buffer buffer;
};


int main(int argc, char *argv[]) {

	Parameters parameters(argc, argv);
	Server server(parameters);
	server.run();

	return 0;
}

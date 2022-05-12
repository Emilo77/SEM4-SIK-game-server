#include "Server.h"

#include <cerrno>
#include <netdb.h>
#include <poll.h>
#include <csignal>
#include <cstring>
#include <unistd.h>


bool finish = false;

/* Obsługa sygnału kończenia */
static void catch_int(int sig) {
	finish = true;
	fprintf(stderr,
	        "Signal %d catched. No new connections will be accepted.\n", sig);
}

class Server {

public:
	explicit Server(Game &game, ServerParameters &parameters) :
			game(game),
			parameters(parameters) {}


	ClientMessageStruct parseJoin() {
		if (read_length < 3)
			return {ClientMessage::Invalid};
		string name = buffer.receive_join(read_length);
		if (name.empty()) {
			return {ClientMessage::Invalid};
		}
		return {ClientMessage::Join, name};
	}

	ClientMessageStruct parsePlaceBomb() {
		if (read_length != PLACEBOMB_INSTR_LENGTH) {
			return {ClientMessage::Invalid};
		}
		return {ClientMessage::PlaceBomb};
	}

	ClientMessageStruct parsePlaceBlock() {
		if (read_length != PLACEBOMB_INSTR_LENGTH) {
			return {ClientMessage::Invalid};
		}
		return {ClientMessage::PlaceBlock};
	}

	ClientMessageStruct parseMove() {
		if (read_length != MOVE_INSTR_LENGTH) {
			return {ClientMessage::Invalid};
		}
		Direction direction = buffer.receive_move();
		if (direction == Direction::InvalidDirection) {
			return {ClientMessage::Invalid};
		}
		return {ClientMessage::Move, buffer.receive_move()};
	}

	ClientMessageStruct handle_received_message() {
		auto message_id = buffer.get_message_id();
		auto message = message_id < 4 ?
		               (ClientMessage) message_id : ClientMessage::Invalid;
		switch (message) {
			case Join:
				return parseJoin();
			case PlaceBomb:
				return parsePlaceBomb();
			case PlaceBlock:
				return parsePlaceBlock();
			case Move:
				return parseMove();
			case Invalid:
				return {ClientMessage::Invalid};
		}
	}

	void apply_message(Player &player) {
		string new_player_name;
		ClientMessageStruct received = handle_received_message();
		ClientMessage clientMessage = received.message;
		switch (clientMessage) {
			case Join:
				break;
			case PlaceBomb:
				break;
			case PlaceBlock:
				break;
			case Move:
				break;
			case Invalid:
				//disconnect śmietnika
				break;
		}
		//send message do użytkowników
	}

	void initialize() {}

	void run() {}

private:
	Game game;
	Buffer buffer;
	ssize_t read_length{0};
	ssize_t sent_length{0};
	ServerParameters &parameters;
	size_t active_clients = 0;
	struct pollfd poll_descriptors[CONNECTIONS];

};


int main(int argc, char *argv[]) {
	ServerParameters parameters(argc, argv);
	Game game(parameters);
	Server server(game, parameters);
	server.run();

	return 0;
}

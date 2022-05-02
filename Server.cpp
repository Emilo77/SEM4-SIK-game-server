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
	explicit Server(Game &game, Parameters &parameters) :
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
	}

	void initialize() {
//		install_signal_handler(SIGINT, catch_int, SA_RESTART);
//		/* Inicjujemy tablicę z gniazdkami klientów, poll_descriptors[0] to gniazdko centrali */
//		for (auto &poll_descriptor: poll_descriptors) {
//			poll_descriptor.fd = -1;
//			poll_descriptor.events = POLLIN;
//			poll_descriptor.revents = 0;
//		}
//		/* Tworzymy gniazdko centrali */
//		poll_descriptors[0].fd = open_socket();
//		bind_socket(poll_descriptors[0].fd, parameters.port);
//		printf("Listening on port %u\n", parameters.port);
//		start_listening(poll_descriptors[0].fd, QUEUE_LENGTH);

	}

	void run() {
//		do {
//			for (auto &poll_descriptor: poll_descriptors) {
//				poll_descriptor.revents = 0;
//			}
//
//			/* Po Ctrl-C zamykamy gniazdko centrali */
//			if (finish && poll_descriptors[0].fd >= 0) {
//				CHECK_ERRNO(close(poll_descriptors[0].fd));
//				poll_descriptors[0].fd = -1;
//			}
//
//			int poll_status = poll(poll_descriptors, CONNECTIONS,
//			                       parameters.turn_duration);
//			if (poll_status == -1) {
//				if (errno == EINTR)
//					fprintf(stderr, "Interrupted system call\n");
//				else
//					PRINT_ERRNO();
//			} else if (poll_status > 0) {
//				if (!finish && (poll_descriptors[0].revents & POLLIN)) {
//					/* Przyjmuję nowe połączenie */
//					sockaddr_in client_address;
//					int client_fd = accept_connection(poll_descriptors[0].fd,
//					                                  &client_address);
//
//					bool accepted = false;
//					for (int i = 1; i < CONNECTIONS; ++i) {
//						if (poll_descriptors[i].fd == -1) {
//							fprintf(stderr, "Received new connection (%d)\n",
//							        i);
//
//							poll_descriptors[i].fd = client_fd;
//							poll_descriptors[i].events = POLLIN;
//							active_clients++;
//							accepted = true;
//							break;
//						}
//					}
//					if (!accepted) {
//						CHECK_ERRNO(close(client_fd));
//						fprintf(stderr, "Too many clients\n");
//					}
//				}
//				for (int i = 1; i < CONNECTIONS; ++i) {
//					if (poll_descriptors[i].fd != -1 &&
//					    (poll_descriptors[i].revents & (POLLIN | POLLERR))) {
//						ssize_t received_bytes = read(poll_descriptors[i].fd,
//						                              buffer.get(), sizeof
//						                              (buffer.get()));
//						if (received_bytes < 0) {
//							fprintf(stderr,
//							        "Error when reading message from connection %d (errno %d, %s)\n",
//							        i, errno, strerror(errno));
//							CHECK_ERRNO(close(poll_descriptors[i].fd));
//							poll_descriptors[i].fd = -1;
//							active_clients -= 1;
//						} else if (received_bytes == 0) {
//							fprintf(stderr, "Ending connection (%d)\n", i);
//							CHECK_ERRNO(close(poll_descriptors[i].fd));
//							poll_descriptors[i].fd = -1;
//							active_clients -= 1;
//						} else {
//							printf("(%d) -->%.*s\n", i, (int) received_bytes,
//							       buffer.get());
//						}
//					}
//				}
//			} else {
//				printf("%lu milliseconds passed without any events\n",
//				       parameters.turn_duration);
//			}
//		} while (!finish || active_clients > 0);
//
//		if (poll_descriptors[0].fd >= 0)
//			CHECK_ERRNO(close(poll_descriptors[0].fd));
//		exit(EXIT_SUCCESS);
	}

private:
	Game game;
	Buffer buffer;
	ssize_t read_length{0};
	ssize_t sent_length{0};
	Parameters &parameters;
	size_t active_clients = 0;
	struct pollfd poll_descriptors[CONNECTIONS];

};


int main(int argc, char *argv[]) {
	Parameters parameters(argc, argv);
	Game game(parameters);
	Server server(game, parameters);
	server.run();

	return 0;
}

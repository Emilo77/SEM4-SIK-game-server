#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <cstring>
#include <sys/types.h>

using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::map;

#define MINIMAL_PARAMS_NUMBER 11
#define BUFFER_SIZE 65507

struct Random {
	uint32_t random;

	explicit Random(uint32_t seed) : random(seed) {}

	uint32_t generate() {
		uint32_t r = random;
		random = (random * 279410273) % 4294967291;
		return r;
	}
};

enum Direction {
	Up = 0,
	Right = 1,
	Down = 2,
	Left = 3,
};

struct Player {
	string name;
	string address;
};

struct Position {
	uint16_t x;
	uint16_t y;

	Position(uint16_t x, uint16_t y): x(x), y(y) {}
};

class Parameters {
private:
	int argc{0};
	char **argv;
public:
	uint16_t bomb_timer{0};
	uint8_t players_count{0};
	uint64_t turn_duration{0};
	uint16_t explosion_radius{0};
	bool help{false};
	uint16_t initial_blocks{0};
	uint16_t game_length{0};
	string server_name{};
	uint16_t port{0};
	uint32_t seed{0};
	uint16_t size_x{0};
	uint16_t size_y{0};

	Parameters(int argc, char *argv[]) : argc(argc), argv(argv) {
		check_parameters();
	}

private:
	enum wrong_parameters {
		TOO_FEW_PARAMETERS,
		WRONG_PARAMETER,
		MISSING_PARAMETER,
	};

	void exit_program(enum wrong_parameters) {
		string expected = "-b <bomb timer> -c <players count> -d <turn duration> "
		                  "-e <explosion radius> -h <help> -k <initial blocks> "
		                  "-l <game length> -n <server name> -p <port> [-s <seed>] "
		                  "-x <size x> -y <size y>";

		cerr << "Usage: " << argv[0] << expected << endl;
	}

	void parse_flag(char flag) {

	}

	void check_parameters() {
		char flags_arr[12] = {'b', 'c', 'd', 'e', 'h', 'k', 'l', 'n', 'p',
		                      's', 'x', 'y'};
		map<char, bool> flag_occured;
		for (char flag: flags_arr) {
			flag_occured.insert({flag, false});
		}

		if (argc < MINIMAL_PARAMS_NUMBER)
			exit_program(TOO_FEW_PARAMETERS);

		const char *flags = "-b:c:d:e:h:k:l:n:p:s:x:y";
		int opt;
		while ((opt = getopt(argc, argv, flags)) != -1) {
			parse_flag((char) opt);
			flag_occured.at((char) opt) = true;
		}

		for (auto x: flag_occured) {
			if (!x.second)
				exit_program(MISSING_PARAMETER);
		}
	}
};

// Class for operations on buffer, mostly converting data to proper format
class Buffer {

private:
	template<typename T>
	T convert_to_send(T number) {
		switch (sizeof(T)) {
			case 1:
				return number;
			case 2:
				return htobe16(number);
			case 4:
				return htobe32(number);
			default:
				return htobe64(number);
		}
	}

	template<typename T>
	T convert_to_receive(T number) {
		switch (sizeof(T)) {
			case 1:
				return number;
			case 2:
				return be16toh(number);
			case 4:
				return be32toh(number);
			default:
				return be64toh(number);
		}
	}

	template<typename T>
	void insert(T number) {
		int size = sizeof(T);
		number = convert_to_send(number);
		memcpy(buffer + send_index, &number, size);
		send_index += size;
	}

	void insert(const string &str) {
		size_t size = str.size();
		memcpy(buffer + send_index, str.c_str(), size);
		send_index += size;
	}

	template<typename T>
	void receive_number(T &number) {
		int size = sizeof(T);
		memcpy(&number, buffer + read_index, size);
		read_index += size;
		number = convert_to_receive(number);
	}

	void receive_string(string &str, size_t str_size) {
		str = {buffer + read_index, str_size};
	}

	// we begin from 1, because we already know the value of buffer[0]- message_id
	void reset_read_index() { read_index = 1; }

	void reset_send_index() { send_index = 0; }

public:

	string receive_join() {
		string name;
		uint8_t name_size;
		receive_number(name_size);
		receive_string(name, name_size);
		return name;
	}

	Direction receive_move() {
		uint8_t direction;
		receive_number(direction);
		return (Direction) direction;
		//todo sprawdzić działa
	}

	size_t get_size() const { return send_index; }

	char get_message_id() { return buffer[0]; }

	char *get() { return buffer; }

private:
	char buffer[BUFFER_SIZE];
	size_t send_index{0};
	size_t read_index{1};
};

class Server {
private:
	Parameters parameters;
	Random random;
	enum game_state {
		lobby_state,
		gameplay_state,
	};

public:
	explicit Server(Parameters &p) : parameters(p), random(parameters.seed) {}

	void run() {}

};


int main(int argc, char *argv[]) {

	Parameters parameters(argc, argv);
	Server server(parameters);
	server.run();

	return 0;
}

#ifndef ZADANIE02_SERVERPARAMETERS_H
#define ZADANIE02_SERVERPARAMETERS_H

#include <boost/program_options.hpp>
#include <cstdint>
#include <iostream>
#include <string>
#include <limits>
#include <chrono>

class ServerParametersParser {
public:
	ServerParametersParser(int argc, char **argv) : argc(argc), argv(argv) {
		check_parameters();
	}

private:
	void check_parameters();

public:
	uint16_t bomb_timer{0};
	uint8_t players_count{0};
	uint64_t turn_duration{0};
	uint16_t explosion_radius{0};
	uint16_t initial_blocks{0};
	uint16_t game_length{0};
	std::string server_name;
	uint16_t port{0};
	uint32_t seed{0};
	uint16_t size_x{0};
	uint16_t size_y{0};

private:
	int argc{0};
	char **argv;
};


#endif //ZADANIE02_SERVERPARAMETERS_H

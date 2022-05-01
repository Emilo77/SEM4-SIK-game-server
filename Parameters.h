#ifndef ZADANIE02_PARAMETERS_H
#define ZADANIE02_PARAMETERS_H

#include <iostream>
#include <string>
#include <map>
#include <getopt.h>

class Parameters {
private:
	int argc{0};
	char **argv;
	enum wrong_parameters {
		WRONG_PARAMETER,
		MISSING_PARAMETER,
		UNKNOWN_FLAG,
	};

public:
	uint16_t bomb_timer{0};
	uint8_t players_count{0};
	uint64_t turn_duration{0};
	uint16_t explosion_radius{0};
	bool help{false};
	uint16_t initial_blocks{0};
	uint16_t game_length{0};
	std::string server_name{};
	uint16_t port{0};
	uint32_t seed{0};
	uint16_t size_x{0};
	uint16_t size_y{0};

	Parameters(int argc, char *argv[]) : argc(argc), argv(argv) {
		check_parameters();
	}

	void exit_program(enum wrong_parameters);

	void check_parameters();
};

#endif //ZADANIE02_PARAMETERS_H

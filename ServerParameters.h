#ifndef ZADANIE02_SERVERPARAMETERS_H
#define ZADANIE02_SERVERPARAMETERS_H

#include <boost/program_options.hpp>
#include <cstdint>
#include <iostream>
#include <string>
#include <limits>

class ServerParameters {
public:
	ServerParameters(int argc, char **argv) : argc(argc), argv(argv) {
		check_parameters();
		print_parameters();
	}

private:
	void check_parameters();

	void print_parameters() const {
		std:: cout << "Server parameters:" << std::endl;
		std:: cout << "         bomb_timer:       " << bomb_timer << std::endl;
		std:: cout << "         players_count:    " << players_count <<std::endl;
		std:: cout << "         turn_duration:    " << turn_duration <<std::endl;
		std:: cout << "         explosion_radius: " << explosion_radius << std::endl;
		std:: cout << "         initial_blocks:   " << initial_blocks <<std::endl;
		std:: cout << "         game_length:      " << game_length << std::endl;
		std:: cout << "         server_name:      " << server_name << std::endl;
		std:: cout << "         port:             " << port << std::endl;
		std:: cout << "         size_x:           " << size_x << std::endl;
		std:: cout << "         size_y:           " << size_y << std::endl;

	}

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

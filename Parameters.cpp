#include "Parameters.h"

using std::string;

void Parameters::exit_program(enum wrong_parameters) {
	string expected = "-b <bomb timer> -c <players count> -d <turn duration> "
	                  "-e <explosion radius> -h <help> -k <initial blocks> "
	                  "-l <game length> -n <server name> -p <port> [-s <seed>] "
	                  "-x <size x> -y <size y>";

	std::cerr << "Usage: " << argv[0] << expected << std::endl;
}

void Parameters::check_parameters() {
	char flags_arr[12] = {'b', 'c', 'd', 'e', 'h', 'k', 'l', 'n', 'p',
	                      's', 'x', 'y'};

	std::map<char, bool> flag_occured;
	for (char flag: flags_arr) {
		flag_occured.insert({flag, false});
	}


	const char *flags = "-b:c:d:e:h:k:l:n:p:s:x:y";
	int opt;
	while ((opt = getopt(argc, argv, flags)) != -1) {
		size_t value = strtoul(optarg, nullptr, 10);
//			if (value == 0) {
//				exit_program(WRONG_PARAMETER);
//			}
		switch (opt) {
			case 'b':
				bomb_timer = value;
				break;
			case 'c':
				players_count = value;
				break;
			case 'd':
				turn_duration = value;
				break;
			case 'e':
				explosion_radius = value;
				break;
			case 'h':
				help = true;
			case 'k':
				initial_blocks = value;
				break;
			case 'l':
				game_length = value;
				break;
			case 'n':
				server_name = optarg; //może trzeba skopiować
				break;
			case 'p':
				port = value;
				break;
			case 's':
				seed = value;
				break;
			case 'x':
				size_x = value;
				break;
			case 'y':
				size_y = value;
				break;
			default:
				exit_program(UNKNOWN_FLAG);
		}
		flag_occured.at((char) opt) = true;
	}

	for (auto x: flag_occured) {
		if (!x.second && x.first != 's')
			exit_program(MISSING_PARAMETER);
	}
}

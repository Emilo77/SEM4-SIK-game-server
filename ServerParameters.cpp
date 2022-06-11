#include "ServerParametersParser.h"

namespace po = boost::program_options;

/* Pomocnicza funkcja do uproszczenia działania biblioteki Boost */
template<class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
	copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
	return os;
}

/* Zakończenie programu w przypadku podania błędnych parametrów */
void exit_program(int status) {
	if (status) {
		std::cerr << "Consider using -h [--help] option" << std::endl;
	}
	exit(status);
}

/* Sprawdzenie, czy liczba mieści się w dozwolonym przedziale */
static inline void check_8(int64_t number, const char *parameter) {
	if ((number < 0) || (number > UINT8_MAX)) {
		throw po::validation_error(po::validation_error::invalid_option_value,
		                           parameter);
	}
}

/* Sprawdzenie, czy liczba mieści się w dozwolonym przedziale */
static inline void check_16(int64_t number, const char *parameter) {
	if ((number < 0) || (number > UINT16_MAX)) {
		throw po::validation_error(po::validation_error::invalid_option_value,
		                           parameter);
	}
}

/* Sprawdzenie, czy liczba mieści się w dozwolonym przedziale */
static inline void check_64(int64_t number, const char *parameter) {
	if ((number < 0) || (number > INT64_MAX)) {
		throw po::validation_error(po::validation_error::invalid_option_value,
		                           parameter);
	}
}

void ServerParametersParser::check_parameters() {
	const po::positional_options_description p;
	po::options_description desc(1024, 512);

	try {
		int64_t bomb_timer_;
		int64_t players_count_;
		int64_t turn_duration_;
		int64_t explosion_radius_;
		int64_t initial_blocks_;
		int64_t game_length_;
		int64_t port_;
		int64_t size_x_;
		int64_t size_y_;
		std::string es;

		desc.add_options()
				("help,h", "produce help message")

				("bomb-timer,b", po::value<int64_t>(&bomb_timer_)->value_name
						("<u16>")->required(), "set bomb timer")

				("players-count,c", po::value<int64_t>(
						&players_count_)->value_name
						("<u8>")->required(), "set number of players")

				("turn-duration,d", po::value<int64_t>(&turn_duration_)
						 ->value_name("<u64, milliseconds>")->required(),
				 "set turn duration")

				("explosion-radius,e", po::value<int64_t>(&explosion_radius_)
						 ->value_name("<u16>")->required(),
				 "set explosion radius")

				("initial-blocks,k", po::value<int64_t>(&initial_blocks_)
						->value_name("<u16>")->required(), "set initial "
				                                           "blocks number")

				("game-length,l", po::value<int64_t>(&game_length_)
						->value_name("<u16>")->required(), "set game length")

				("server-name,n", po::value<std::string>(&server_name)
						->value_name("<String>")->required(), "set server name")

				("port,p", po::value<int64_t>(&port_)
						->value_name("<u16>")->required(), "set port")

				("seed,s", po::value<uint32_t>(&seed)
						 ->value_name("<u32, optional parameter>")
						 ->default_value(static_cast<uint32_t>
						                 (std::chrono::system_clock::now().time_since_epoch().count())),
				 "set seed")

				("size-x,x", po::value<int64_t>(&size_x_)
						->value_name("<u16>")->required(), "set size x of "
				                                           "board")

				("size-y,y", po::value<int64_t>(&size_y_)
						->value_name("<u16>")->required(), "set size y of "
				                                           "board");

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv)
				          .options(desc)
				          .positional(p)
				          .run(), vm);

		/* Jeżeli wystąpi opcja help, ignorujemy inne parametry
		 * i kończymy działanie programu */
		if (vm.count("help")) {
			std::cout << "Program Usage: ./" << argv[0] << "\n" << desc << "\n";
			exit_program(EXIT_SUCCESS);
		}

		/* Jeżeli obowiązkowy parametr nie został podany,
		 * zostanie rzucony wyjątek.*/
		po::notify(vm);

		/* Sprawdzenie poprawności zakresów parametrów,
		 * w przypadku błędu zostanie rzucony wyjątek. */
		check_16(bomb_timer_, "bomb-timer");
		check_8(players_count_, "players-count");
		check_64(turn_duration_, "turn-duration");
		check_16(explosion_radius_, "explosion-radius");
		check_16(initial_blocks_, "initial-blocks");
		check_16(game_length_, "game-length");
		check_16(port_, "port");
		check_16(size_x_, "size-x");
		check_16(size_y_, "size-y");

		/* Ustawienie odpowiednich typów. */
		bomb_timer = (uint16_t) bomb_timer_;
		players_count = (uint8_t) players_count_;
		turn_duration = (uint64_t) turn_duration_;
		explosion_radius = (uint16_t) explosion_radius_;
		initial_blocks = (uint16_t) initial_blocks_;
		game_length = (uint16_t) game_length_;
		port = (uint16_t) port_;
		size_x = (uint16_t) size_x_;
		size_y = (uint16_t) size_y_;

	}
	/* W przypadku złapania wyjątku program wypisuje błąd
	 * oraz kończy działanie. */
	catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << "\n";
		exit_program(EXIT_FAILURE);
	}
	catch (...) {
		std::cerr << "Unknown error!" << "\n";
		exit_program(EXIT_FAILURE);
	}


}

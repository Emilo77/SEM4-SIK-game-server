#ifndef ZADANIE02_GAME_H
#define ZADANIE02_GAME_H

#include "Utils.h"
#include <set>

class Game {
	enum game_state {
		lobby_state,
		gameplay_state,
	};

	int round_number{0};
	std::list<Event> events;
	std::set<Bomb> bombs;
	std::set<Position> blocks;
};


#endif //ZADANIE02_GAME_H

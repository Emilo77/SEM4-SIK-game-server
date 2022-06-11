#ifndef ZADANIE02_SERVERPARAMETERS_H
#define ZADANIE02_SERVERPARAMETERS_H

#include <boost/program_options.hpp>
#include <iostream>
#include <limits>
#include <chrono>

#include "ServerParameters.h"

class ServerParametersParser {
public:
	ServerParametersParser(int argc, char **argv) : argc(argc), argv(argv) {}

	ServerParameters check_parameters();
private:
	int argc{0};
	char **argv;
};


#endif //ZADANIE02_SERVERPARAMETERS_H

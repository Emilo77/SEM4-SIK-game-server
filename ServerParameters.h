#ifndef ZADANIE02_SERVER_SERVERPARAMETERS_H
#define ZADANIE02_SERVER_SERVERPARAMETERS_H

#include <cstdint>
#include <string>
#include <utility>

class ServerParameters {
public:
	ServerParameters(uint16_t bombTimer, uint8_t playersCount,
	                 uint64_t turnDuration, uint16_t explosionRadius,
	                 uint16_t initialBlocks, uint16_t gameLength,
	                 std::string serverName, uint16_t port, uint32_t seed,
	                 uint16_t sizeX, uint16_t sizeY) :
			bomb_timer(bombTimer),
			players_count(playersCount),
			turn_duration(turnDuration),
			explosion_radius(explosionRadius),
			initial_blocks(initialBlocks),
			game_length(gameLength),
			server_name(std::move(serverName)),
			port(port), seed(seed),
			size_x(sizeX),
			size_y(sizeY) {}

	[[nodiscard]] uint16_t getBombTimer() const { return bomb_timer; }

	[[nodiscard]] uint8_t getPlayersCount() const { return players_count; }

	[[nodiscard]] uint64_t getTurnDuration() const { return turn_duration; }

	[[nodiscard]] uint16_t getExplosionRadius() const { return explosion_radius; }

	[[nodiscard]] uint16_t getInitialBlocks() const { return initial_blocks; }

	[[nodiscard]] uint16_t getGameLength() const { return game_length; }

	[[nodiscard]] const std::string &getServerName() const { return server_name; }

	[[nodiscard]] uint16_t getPort() const { return port; }

	[[nodiscard]] uint32_t getSeed() const { return seed; }

	[[nodiscard]] uint16_t getSizeX() const { return size_x; }

	[[nodiscard]] uint16_t getSizeY() const { return size_y; }

private:
	uint16_t bomb_timer;
	uint8_t players_count;
	uint64_t turn_duration;
	uint16_t explosion_radius;
	uint16_t initial_blocks;
	uint16_t game_length;
	std::string server_name;
	uint16_t port;
	uint32_t seed;
	uint16_t size_x;
	uint16_t size_y;
};

#endif //ZADANIE02_SERVER_SERVERPARAMETERS_H

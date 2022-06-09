#ifndef ZADANIE02_CLIENT_MESSAGES_H
#define ZADANIE02_CLIENT_MESSAGES_H

#include <variant>
#include <string>
#include <map>
#include "Utils.h"

/* Typ wiadomości wysyłanej od klienta do serwera. */
enum ClientMessageToServerType {
	Join = 0,
	PlaceBomb = 1,
	PlaceBlock = 2,
	Move = 3,
};

/* Typ wiadomości wysyłanej od serwera do klienta. */
enum ServerMessageToClientType {
	Hello = 0,
	AcceptedPlayer = 1,
	GameStarted = 2,
	Turn = 3,
	GameEnded = 4,
};

/* Wiadomość Hello */
struct Hello {
	std::string server_name;
	player_id_t players_count;
	uint16_t size_x;
	uint16_t size_y;
	uint16_t game_length;
	uint16_t explosion_radius;
	uint16_t bomb_timer;

	Hello(std::string &serverName, player_id_t playersCount,
	      uint16_t sizeX, uint16_t sizeY, uint16_t gameLength,
	      uint16_t explosionRadius, uint16_t bombTimer)
			: server_name(serverName),
			  players_count(playersCount),
			  size_x(sizeX),
			  size_y(sizeY),
			  game_length(gameLength),
			  explosion_radius(explosionRadius),
			  bomb_timer(bombTimer) {}
};

/* Wiadomość AcceptedPlayer */
struct AcceptedPlayer {
	player_id_t player_id{};
	Player player{};
};

/* Wiadomość GameStarted */
struct GameStarted {
	std::map<player_id_t, Player> players;

	GameStarted(std::map<player_id_t, Player> &players)
			: players(players) {}
};

/* Wiadomość Turn */
struct Turn {
	uint16_t turn_number;
	std::vector<Event> events;

	Turn(uint16_t turnNumber, std::vector<Event> &events)
			: turn_number(turnNumber),
			  events(events) {}
};

/* Wiadomość GameEnded */
struct GameEnded {
	std::map<player_id_t, score_t> scores;
};

/* Struktura wiadomości wysyłanej od serwera do klienta. */
typedef struct ServerMessage {
	ServerMessageToClientType type;
	std::variant<struct Hello, struct AcceptedPlayer,
			struct GameStarted, struct Turn, struct GameEnded>
			data;

	ServerMessage(ServerMessageToClientType type,
	              std::variant<struct Hello, struct AcceptedPlayer,
			              struct GameStarted, struct Turn, struct GameEnded> data)
			: type(type),
			  data(std::move(data)) {}

} ServerMessage;

/* Struktura wiadomości wysyłanej od klienta do serwera. */
typedef struct ClientMessage {
	std::optional<player_id_t> player_id;
	ClientMessageToServerType type;
	std::variant<std::string, Direction> data;

	ClientMessage(ClientMessageToServerType type,
	              std::variant<std::string, Direction> data)
			: type(type),
			  data(std::move(data)) {}
} ClientMessage;

#endif //ZADANIE02_CLIENT_MESSAGES_H

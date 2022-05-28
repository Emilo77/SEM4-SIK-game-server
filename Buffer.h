#ifndef ZADANIE02_SERVER_BUFFER_H
#define ZADANIE02_SERVER_BUFFER_H


/* Początkowa wielkość bufora, aby móc odebrać największy możliwy pakiet TCP */
#define MAX_PACKAGE_SIZE 65535
/* Początkowa wielkość bufora do obsługi GUI -> SERWER, są tam wysyłane komunikaty
 * o małej wielkości, zatem ustawiamy mniejszy rozmiar bufora. */
#define SMALL_BUFFER_SIZE 64

#include "ServerParameters.h"
#include "Utils.h"
#include "Messages.h"
#include <cstring>
#include <optional>

#include <exception>

using std::string;

struct InvalidMessage : public std::exception {
	[[nodiscard]] const char *what() const noexcept override {
		return "Incorrect message";
	}
};

struct IncompleteMessage : public std::exception {
	[[nodiscard]] const char *what() const noexcept override {
		return "Incomplete message";
	}
};

class Buffer {
private:
	/* Sprawdzenie, czy wiadomość przyszła niepełna */
	void check_if_message_incomplete(size_t variable) const;

	/* Konwertowanie liczby przed wysłaniem wiadomości */
	static uint16_t convert_to_send(uint16_t number);

	static uint32_t convert_to_send(uint32_t number);

	/* Konwertowanie liczby po odebraniu wiadomości */
	static uint16_t convert_to_receive(uint16_t number);

	static uint32_t convert_to_receive(uint32_t number);

	/* Wstawianie napisu (bez jego długości) */
	void insert_raw(const std::string &str);

	/* Odbieranie napisu */
	void receive_raw(std::string &str, size_t str_size);

	/* Wstawianie typów liczbowych */
	void insert(uint8_t number);

	void insert(uint16_t number);

	void insert(uint32_t number);

	/* Wstawianie obiektów do bufora zgodnie z konwencją zadania */
	void insert(const std::string &str);

	void insert(Position &position);

	void insert(Player &player);

	/* Odbieranie zgodnie z konwencją zadania  */
	void receive(uint8_t &number);

	void receive(uint16_t &number);

	void receive(uint32_t &number);

	void receive(std::string &str);

	/* Wstawianie eventu */
	void insert_bomb_placed(struct BombPlaced &bomb_placed);

	void insert_bomb_exploded(struct BombExploded &bomb_exploded);

	void insert_player_moved(struct PlayerMoved &player_moved);

	void insert_block_placed(struct BlockPlaced &block_placed);

	void insert_event(Event &event);

	/* Wstawianie list */
	void insert_list_events(std::vector<Event> &vector);

	void insert_list_player_ids(std::vector<player_id_t> &ids);

	void insert_list_positions(std::vector<Position> &positions);

	/* Wstawianie map */
	void insert_map_players(std::map<player_id_t, Player> &players);

	void insert_map_scores(std::map<player_id_t, score_t> &scores);

	/* Wstawianie wiadomości wysyłanych od graczy */
	void insert_hello(struct Hello &hello);

	void insert_accepted_player(struct AcceptedPlayer &accepted_player);

	void insert_game_started(struct GameStarted &game_started);

	void insert_turn(struct Turn &turn);

	void insert_game_ended(struct GameEnded &game_ended);

	/* Przywrócenie indeksu przed odbieraniem nowej wiadomości */
	void reset_read_index() { read_index = 0; }

	/* Przywrócenie indeksu przed wysyłaniem nowej wiadomości */
	void reset_send_index() { send_index = 0; }

	/* Ustawienie indeksu na odbiór danych z funkcji Receive.
	 * Jeżeli pakiet ma szanse się nie zmieścić, zwiększamy rozmiar kontenera. */
	void set_shift(size_t value) {
		shift_index = value;
		adapt_size();
	}

	/* Zwrócenie wielkości zapisanej wiadomości do bufora */
	[[nodiscard]] size_t get_send_size() const { return send_index; }

	/* Zwrócenie wielkości odebranej wiadomości z bufora */
	[[nodiscard]] size_t get_read_size() const { return read_index; }

public:
	/* Wstępne ustawienie wielkości kontenerów. */
	void initialize(size_t size);

	/* Wstępne ustawienie wielkości kontenerów. */
	void adapt_size();

	/* Wstawianie wiadomości wysyłanej do serwera */
	size_t insert_msg_to_server(ClientMessageToServer &message);

	/* Odbieranie wiadomości wysyłanej od serwera */
	std::optional<ServerMessageToClient>
	receive_msg_from_server(size_t received_size);

	/* Referencja do bufora odbierającego komunikaty */
	char *get_receive() { return &receive_buffer[shift_index]; }

	/* Referencja do bufora wysyłającego komunikaty */
	char *get_send() { return &send_buffer[0]; }

private:
	std::vector<char> receive_buffer;
	std::vector<char> send_buffer;
	size_t send_index{0};
	size_t read_index{0};
	size_t shift_index{0};
	size_t end_of_data_index{0};
};


#endif //ZADANIE02_SERVER_BUFFER_H

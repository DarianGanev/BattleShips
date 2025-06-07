#ifndef BATTLESHIPS_H
#define BATTLESHIPS_H

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN "cls"
#else
#include <unistd.h>
#define CLEAR_SCREEN "clear"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_SIZE 10
#define MAX_SHIPS 10

typedef enum {
    SMALL_SHIP = 2,
    MEDIUM_SHIP = 3,
    LARGE_SHIP = 4,
    CRUISER = 6
} ShipType;

typedef enum {
    HORIZONTAL = 0,
    VERTICAL = 1
} Direction;

typedef enum {
    EMPTY = 0,
    SHIP = 1,
    HIT = 2,
    MISS = 3
} CellState;

typedef struct {
    int row;
    int col;
    ShipType length;
    Direction direction;
    int hits;
    int is_sunk;
} Ship;

typedef struct {
    char name[50];
    CellState board[BOARD_SIZE][BOARD_SIZE];
    CellState attack_board[BOARD_SIZE][BOARD_SIZE];
    Ship ships[MAX_SHIPS];
    int ship_count;
    int ships_remaining;
} Player;

typedef struct {
    Player player1;
    Player player2;
    int current_player;
    int game_over;
    int winner;
    int last_attack_row;
    int last_attack_col;
} Game;

void init_player(Player* player, const char* name);
void init_game(Game* game);
int is_valid_position(Player* player, int row, int col, ShipType length, Direction direction);
int place_ship(Player* player, int row, int col, ShipType length, Direction direction);
void setup_ships_manual(Player* player);
void setup_ships_from_file(Player* player, const char* filename);
int make_attack(Player* attacker, Player* defender, int row, int col);
void start_multiplayer_game(void);
void start_singleplayer_game(void);
void print_board(CellState board[BOARD_SIZE][BOARD_SIZE], int show_ships);
void clear_screen(void);
char get_column_letter(int col);
int get_column_number(char letter);
void show_ship_setup_menu(Player* player);
void show_attack_menu(Player* current, Player* opponent, Game* game);
void clear_input_buffer(void);
void ai_place_ships(Player* player);
int ai_make_attack(Player* attacker, Player* defender, Game* game);
int save_game(Game* game, const char* filename);
int load_game(Game* game, const char* filename);
void simple_encrypt_decrypt(char* data, int length, const char* key);

#endif
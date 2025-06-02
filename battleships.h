#ifndef BATTLESHIPS_H
#define BATTLESHIPS_H

#define BOARD_SIZE 10
#define MAX_SHIPS 10

// Типове кораби
typedef enum {
    SMALL_SHIP = 2,   // 4 кораба с дължина 2
    MEDIUM_SHIP = 3,  // 3 кораба с дължина 3
    LARGE_SHIP = 4,   // 2 кораба с дължина 4
    CRUISER = 6       // 1 кораб с дължина 6
} ShipType;

// Посоки
typedef enum {
    HORIZONTAL = 0,
    VERTICAL = 1
} Direction;

// Състояние на клетка
typedef enum {
    EMPTY = 0,
    SHIP = 1,
    HIT = 2,
    MISS = 3
} CellState;

// Структура за кораб
typedef struct {
    int row;
    int col;
    ShipType length;
    Direction direction;
    int hits;
    int is_sunk;
} Ship;

// Структура за играч
typedef struct {
    char name[50];
    CellState board[BOARD_SIZE][BOARD_SIZE];
    CellState attack_board[BOARD_SIZE][BOARD_SIZE];
    Ship ships[MAX_SHIPS];
    int ship_count;
    int ships_remaining;
} Player;

// Структура за игра
typedef struct {
    Player player1;
    Player player2;
    int current_player;
    int game_over;
    int winner;
} Game;

// Функции за инициализация
void init_player(Player* player, const char* name);
void init_game(Game* game);

// Функции за поставяне на кораби
int is_valid_position(Player* player, int row, int col, ShipType length, Direction direction);
int place_ship(Player* player, int row, int col, ShipType length, Direction direction);
void setup_ships_manual(Player* player);
void setup_ships_from_file(Player* player, const char* filename);

// Функции за играта
int make_attack(Player* attacker, Player* defender, int row, int col);
void start_multiplayer_game();
void start_singleplayer_game();

// Помощни функции
void print_board(CellState board[BOARD_SIZE][BOARD_SIZE], int show_ships);
void clear_screen();
char get_column_letter(int col);
int get_column_number(char letter);

#endif

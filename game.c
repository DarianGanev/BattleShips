#include "battleships.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_player(Player* player, const char* name) {
    strcpy(player->name, name);
    player->ship_count = 0;
    player->ships_remaining = MAX_SHIPS;
    
    // Инициализиране на дъските
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            player->board[i][j] = EMPTY;
            player->attack_board[i][j] = EMPTY;
        }
    }
    
    // Инициализиране на корабите
    for(int i = 0; i < MAX_SHIPS; i++) {
        player->ships[i].hits = 0;
        player->ships[i].is_sunk = 0;
    }
}

void init_game(Game* game) {
    game->current_player = 0;
    game->game_over = 0;
    game->winner = -1;
}

int is_valid_position(Player* player, int row, int col, ShipType length, Direction direction) {
    // Проверка дали корабът е в границите на дъската
    if(direction == HORIZONTAL) {
        if(col + length > BOARD_SIZE) return 0;
    } else {
        if(row + length > BOARD_SIZE) return 0;
    }
    
    // Проверка дали има свободно място
    for(int i = 0; i < length; i++) {
        int check_row = (direction == VERTICAL) ? row + i : row;
        int check_col = (direction == HORIZONTAL) ? col + i : col;
        
        if(player->board[check_row][check_col] != EMPTY) {
            return 0;
        }
    }
    
    // Проверка дали няма съседни кораби (включително диагонално)
    for(int i = -1; i <= length; i++) {
        for(int j = -1; j <= 1; j++) {
            int check_row = (direction == VERTICAL) ? row + i : row + j;
            int check_col = (direction == HORIZONTAL) ? col + i : col + j;
            
            if(check_row >= 0 && check_row < BOARD_SIZE && 
               check_col >= 0 && check_col < BOARD_SIZE) {
                if(player->board[check_row][check_col] == SHIP) {
                    return 0;
                }
            }
        }
    }
    
    return 1;
}

int place_ship(Player* player, int row, int col, ShipType length, Direction direction) {
    if(!is_valid_position(player, row, col, length, direction)) {
        return 0;
    }
    
    // Поставяне на кораба
    Ship* ship = &player->ships[player->ship_count];
    ship->row = row;
    ship->col = col;
    ship->length = length;
    ship->direction = direction;
    ship->hits = 0;
    ship->is_sunk = 0;
    
    // Маркиране на клетките на дъската
    for(int i = 0; i < length; i++) {
        int ship_row = (direction == VERTICAL) ? row + i : row;
        int ship_col = (direction == HORIZONTAL) ? col + i : col;
        player->board[ship_row][ship_col] = SHIP;
    }
    
    player->ship_count++;
    return 1;
}

void print_board(CellState board[BOARD_SIZE][BOARD_SIZE], int show_ships) {
    printf("   ");
    for(int i = 0; i < BOARD_SIZE; i++) {
        printf("%c ", 'A' + i);
    }
    printf("\n");
    
    for(int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", i + 1);
        for(int j = 0; j < BOARD_SIZE; j++) {
            switch(board[i][j]) {
                case EMPTY:
                    printf(". ");
                    break;
                case SHIP:
                    printf("%s ", show_ships ? "S" : ".");
                    break;
                case HIT:
                    printf("X ");
                    break;
                case MISS:
                    printf("O ");
                    break;
            }
        }
        printf("\n");
    }
}

void clear_screen() {
    system("cls");
}

char get_column_letter(int col) {
    return 'A' + col;
}

int get_column_number(char letter) {
    if(letter >= 'A' && letter <= 'J') {
        return letter - 'A';
    } else if(letter >= 'a' && letter <= 'j') {
        return letter - 'a';
    }
    return -1;
}

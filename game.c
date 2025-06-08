#include "battleships.h"

void init_player(Player* player, const char* name) {
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    player->ship_count = 0;
    player->ships_remaining = MAX_SHIPS;
    
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            player->board[i][j] = EMPTY;
            player->attack_board[i][j] = EMPTY;
        }
    }
    
    for(int i = 0; i < MAX_SHIPS; i++) {
        player->ships[i].hits = 0;
        player->ships[i].is_sunk = 0;
    }
}

void init_game(Game* game) {
    game->current_player = 0;
    game->game_over = 0;
    game->winner = -1;
    game->last_attack_row = -1;
    game->last_attack_col = -1;
}

int is_valid_position(Player* player, int row, int col, ShipType length, Direction direction) {
    if(direction == HORIZONTAL) {
        if(col + length > BOARD_SIZE) return 0;
    } else {
        if(row + length > BOARD_SIZE) return 0;
    }
    
    for(int i = 0; i < length; i++) {
        int check_row = (direction == VERTICAL) ? row + i : row;
        int check_col = (direction == HORIZONTAL) ? col + i : col;
        
        if(player->board[check_row][check_col] != EMPTY) {
            return 0;
        }
    }
    
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
    
    Ship* ship = &player->ships[player->ship_count];
    ship->row = row;
    ship->col = col;
    ship->length = length;
    ship->direction = direction;
    ship->hits = 0;
    ship->is_sunk = 0;
    
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

void clear_screen(void) {
    system(CLEAR_SCREEN);
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

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ai_place_ships(Player* player) {
    int ship_types[] = {2, 2, 2, 2, 3, 3, 3, 4, 4, 6};
    
    printf("AI is placing ships...\n");
    
    for (int i = 0; i < MAX_SHIPS; i++) {
        int placed = 0;
        int attempts = 0;
        const int max_attempts = 1000;
        
        while (!placed && attempts < max_attempts) {
            int row = rand() % BOARD_SIZE;
            int col = rand() % BOARD_SIZE;
            Direction direction = rand() % 2;
            
            if (place_ship(player, row, col, ship_types[i], direction)) {
                placed = 1;
            }
            attempts++;
        }
        
        if (!placed) {
            printf("AI failed to place ship %d after %d attempts\n", i + 1, max_attempts);
            break;
        }
    }
    
    printf("AI has placed all ships!\n");
}

int ai_make_attack(Player* attacker, Player* defender, Game* game) {
    static int hunt_mode = 0;
    static int hunt_row = -1, hunt_col = -1;
    static int hunt_direction = 0;
    
    int row, col;
    
    if (hunt_mode && hunt_row >= 0 && hunt_col >= 0) {
        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        int found_target = 0;
        
        for (int i = 0; i < 4 && !found_target; i++) {
            int try_dir = (hunt_direction + i) % 4;
            row = hunt_row + directions[try_dir][0];
            col = hunt_col + directions[try_dir][1];
            
            if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE &&
                attacker->attack_board[row][col] == EMPTY) {
                found_target = 1;
                hunt_direction = try_dir;
            }
        }
        
        if (!found_target) {
            hunt_mode = 0;
        }
    }
    
    if (!hunt_mode) {
        do {
            row = rand() % BOARD_SIZE;
            col = rand() % BOARD_SIZE;
        } while (attacker->attack_board[row][col] != EMPTY);
    }
    
    int result = make_attack(attacker, defender, row, col);
    
    printf("AI attacks %c%d: ", 'A' + col, row + 1);
    
    switch (result) {
        case 0:
            printf("Miss!\n");
            hunt_mode = 0;
            break;
        case 1:
            printf("Hit!\n");
            hunt_mode = 1;
            hunt_row = row;
            hunt_col = col;
            hunt_direction = 0;
            break;
        case 2:
            printf("Hit and sunk!\n");
            hunt_mode = 0;
            break;
        default:
            hunt_mode = 0;
            break;
    }
    
    return result;
}

// Simple XOR encryption/decryption function
void simple_encrypt_decrypt(char* data, int length, const char* key) {
    int key_len = strlen(key);
    for(int i = 0; i < length; i++) {
        data[i] ^= key[i % key_len];
    }
}

int save_game(Game* game, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Cannot create save file '%s'\n", filename);
        return 0;
    }
    
    // Simple encryption key
    const char* key = "BATTLESHIP2024";
    
    // Convert game data to string for encryption
    char buffer[4096];
    int pos = 0;
    
    // Write game state
    pos += sprintf(buffer + pos, "GAME_DATA\n");
    pos += sprintf(buffer + pos, "current_player:%d\n", game->current_player);
    pos += sprintf(buffer + pos, "game_over:%d\n", game->game_over);
    pos += sprintf(buffer + pos, "winner:%d\n", game->winner);
    pos += sprintf(buffer + pos, "last_attack:%d,%d\n", game->last_attack_row, game->last_attack_col);
    
    // Write player 1 data
    pos += sprintf(buffer + pos, "PLAYER1:%s\n", game->player1.name);
    pos += sprintf(buffer + pos, "ship_count:%d\n", game->player1.ship_count);
    pos += sprintf(buffer + pos, "ships_remaining:%d\n", game->player1.ships_remaining);
    
    // Write player 1 ships
    for(int i = 0; i < game->player1.ship_count; i++) {
        Ship* ship = &game->player1.ships[i];
        pos += sprintf(buffer + pos, "ship:%d,%d,%d,%d,%d,%d\n", 
                       ship->row, ship->col, ship->length, ship->direction, ship->hits, ship->is_sunk);
    }
    
    // Write player 1 boards
    pos += sprintf(buffer + pos, "BOARD1:\n");
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            pos += sprintf(buffer + pos, "%d", game->player1.board[i][j]);
        }
        pos += sprintf(buffer + pos, "\n");
    }
    
    pos += sprintf(buffer + pos, "ATTACK_BOARD1:\n");
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            pos += sprintf(buffer + pos, "%d", game->player1.attack_board[i][j]);
        }
        pos += sprintf(buffer + pos, "\n");
    }
    
    // Write player 2 data
    pos += sprintf(buffer + pos, "PLAYER2:%s\n", game->player2.name);
    pos += sprintf(buffer + pos, "ship_count:%d\n", game->player2.ship_count);
    pos += sprintf(buffer + pos, "ships_remaining:%d\n", game->player2.ships_remaining);
    
    // Write player 2 ships
    for(int i = 0; i < game->player2.ship_count; i++) {
        Ship* ship = &game->player2.ships[i];
        pos += sprintf(buffer + pos, "ship:%d,%d,%d,%d,%d,%d\n", 
                       ship->row, ship->col, ship->length, ship->direction, ship->hits, ship->is_sunk);
    }
    
    // Write player 2 boards
    pos += sprintf(buffer + pos, "BOARD2:\n");
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            pos += sprintf(buffer + pos, "%d", game->player2.board[i][j]);
        }
        pos += sprintf(buffer + pos, "\n");
    }
    
    pos += sprintf(buffer + pos, "ATTACK_BOARD2:\n");
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            pos += sprintf(buffer + pos, "%d", game->player2.attack_board[i][j]);
        }
        pos += sprintf(buffer + pos, "\n");
    }
    
    // Encrypt the data
    simple_encrypt_decrypt(buffer, pos, key);
    
    // Write encrypted data to file
    fwrite(buffer, 1, pos, file);
    fclose(file);
    
    printf("Game saved successfully to '%s'\n", filename);
    return 1;
}

int load_game(Game* game, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open save file '%s'\n", filename);
        return 0;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read encrypted data
    char buffer[4096];
    fread(buffer, 1, file_size, file);
    fclose(file);
    
    // Decrypt the data
    const char* key = "BATTLESHIP2024";
    simple_encrypt_decrypt(buffer, file_size, key);
    buffer[file_size] = '\0';
    
    // Parse the decrypted data
    char* line = strtok(buffer, "\n");
    int reading_board1 = 0, reading_attack1 = 0, reading_board2 = 0, reading_attack2 = 0;
    int board_row = 0;
    
    while(line != NULL) {
        if(strstr(line, "GAME_DATA") != NULL) {
            // Skip header
        } else if(strstr(line, "current_player:") != NULL) {
            sscanf(line, "current_player:%d", &game->current_player);
        } else if(strstr(line, "game_over:") != NULL) {
            sscanf(line, "game_over:%d", &game->game_over);
        } else if(strstr(line, "winner:") != NULL) {
            sscanf(line, "winner:%d", &game->winner);
        } else if(strstr(line, "last_attack:") != NULL) {
            sscanf(line, "last_attack:%d,%d", &game->last_attack_row, &game->last_attack_col);
        } else if(strstr(line, "PLAYER1:") != NULL) {
            sscanf(line, "PLAYER1:%s", game->player1.name);
        } else if(strstr(line, "PLAYER2:") != NULL) {
            sscanf(line, "PLAYER2:%s", game->player2.name);
        } else if(strstr(line, "BOARD1:") != NULL) {
            reading_board1 = 1;
            reading_attack1 = reading_board2 = reading_attack2 = 0;
            board_row = 0;
        } else if(strstr(line, "ATTACK_BOARD1:") != NULL) {
            reading_attack1 = 1;
            reading_board1 = reading_board2 = reading_attack2 = 0;
            board_row = 0;
        } else if(strstr(line, "BOARD2:") != NULL) {
            reading_board2 = 1;
            reading_board1 = reading_attack1 = reading_attack2 = 0;
            board_row = 0;
        } else if(strstr(line, "ATTACK_BOARD2:") != NULL) {
            reading_attack2 = 1;
            reading_board1 = reading_attack1 = reading_board2 = 0;
            board_row = 0;
        } else if(reading_board1 && board_row < BOARD_SIZE) {
            for(int j = 0; j < BOARD_SIZE && j < strlen(line); j++) {
                game->player1.board[board_row][j] = line[j] - '0';
            }
            board_row++;
        } else if(reading_attack1 && board_row < BOARD_SIZE) {
            for(int j = 0; j < BOARD_SIZE && j < strlen(line); j++) {
                game->player1.attack_board[board_row][j] = line[j] - '0';
            }
            board_row++;
        } else if(reading_board2 && board_row < BOARD_SIZE) {
            for(int j = 0; j < BOARD_SIZE && j < strlen(line); j++) {
                game->player2.board[board_row][j] = line[j] - '0';
            }
            board_row++;
        } else if(reading_attack2 && board_row < BOARD_SIZE) {
            for(int j = 0; j < BOARD_SIZE && j < strlen(line); j++) {
                game->player2.attack_board[board_row][j] = line[j] - '0';
            }
            board_row++;
        }
        // Note: Ship data parsing omitted for brevity - would need additional parsing logic
        
        line = strtok(NULL, "\n");
    }
    
    printf("Game loaded successfully from '%s'\n", filename);
    return 1;
}

void preview_game(const Game* game) {
    printf("\n=== GAME PREVIEW ===\n");
    printf("Player 1: %s\n", game->player1.name);
    printf("Player 2: %s\n", game->player2.name);
    printf("Winner: ");
    if (game->winner == 0) printf("%s\n", game->player1.name);
    else if (game->winner == 1) printf("%s\n", game->player2.name);
    else printf("None\n");
    printf("\nPlayer 1 board (ships shown):\n");
    print_board(game->player1.board, 1);
    printf("\nPlayer 2 board (ships shown):\n");
    print_board(game->player2.board, 1);
}

int save_game_with_password(Game* game, const char* filename, const char* password) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Cannot create save file '%s'\n", filename);
        return 0;
    }
    char buffer[4096];
    int pos = 0;
    pos += sprintf(buffer + pos, "GAME_DATA\n");
    pos += sprintf(buffer + pos, "current_player:%d\n", game->current_player);
    pos += sprintf(buffer + pos, "game_over:%d\n", game->game_over);
    pos += sprintf(buffer + pos, "winner:%d\n", game->winner);
    pos += sprintf(buffer + pos, "last_attack:%d,%d\n", game->last_attack_row, game->last_attack_col);
    pos += sprintf(buffer + pos, "PLAYER1:%s\n", game->player1.name);
    pos += sprintf(buffer + pos, "ship_count:%d\n", game->player1.ship_count);
    pos += sprintf(buffer + pos, "ships_remaining:%d\n", game->player1.ships_remaining);
    for(int i = 0; i < game->player1.ship_count; i++) {
        Ship* ship = &game->player1.ships[i];
        pos += sprintf(buffer + pos, "ship:%d,%d,%d,%d,%d,%d\n", ship->row, ship->col, ship->length, ship->direction, ship->hits, ship->is_sunk);
    }
    pos += sprintf(buffer + pos, "BOARD1:\n");
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            pos += sprintf(buffer + pos, "%d", game->player1.board[i][j]);
        }
        pos += sprintf(buffer + pos, "\n");
    }
    pos += sprintf(buffer + pos, "ATTACK_BOARD1:\n");
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            pos += sprintf(buffer + pos, "%d", game->player1.attack_board[i][j]);
        }
        pos += sprintf(buffer + pos, "\n");
    }
    pos += sprintf(buffer + pos, "PLAYER2:%s\n", game->player2.name);
    pos += sprintf(buffer + pos, "ship_count:%d\n", game->player2.ship_count);
    pos += sprintf(buffer + pos, "ships_remaining:%d\n", game->player2.ships_remaining);
    for(int i = 0; i < game->player2.ship_count; i++) {
        Ship* ship = &game->player2.ships[i];
        pos += sprintf(buffer + pos, "ship:%d,%d,%d,%d,%d,%d\n", ship->row, ship->col, ship->length, ship->direction, ship->hits, ship->is_sunk);
    }
    pos += sprintf(buffer + pos, "BOARD2:\n");
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            pos += sprintf(buffer + pos, "%d", game->player2.board[i][j]);
        }
        pos += sprintf(buffer + pos, "\n");
    }
    pos += sprintf(buffer + pos, "ATTACK_BOARD2:\n");
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            pos += sprintf(buffer + pos, "%d", game->player2.attack_board[i][j]);
        }
        pos += sprintf(buffer + pos, "\n");
    }
    simple_encrypt_decrypt(buffer, pos, password);
    fwrite(buffer, 1, pos, file);
    fclose(file);
    printf("Game saved successfully to '%s'\n", filename);
    return 1;
}

int load_game_with_password(Game* game, const char* filename, const char* password) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open save file '%s'\n", filename);
        return 0;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char buffer[4096];
    fread(buffer, 1, file_size, file);
    fclose(file);
    simple_encrypt_decrypt(buffer, file_size, password);
    buffer[file_size] = '\0';
    char* line = strtok(buffer, "\n");
    int reading_board1 = 0, reading_attack1 = 0, reading_board2 = 0, reading_attack2 = 0;
    int board_row = 0;
    while(line != NULL) {
        if(strstr(line, "GAME_DATA") != NULL) {
        } else if(strstr(line, "current_player:") != NULL) {
            sscanf(line, "current_player:%d", &game->current_player);
        } else if(strstr(line, "game_over:") != NULL) {
            sscanf(line, "game_over:%d", &game->game_over);
        } else if(strstr(line, "winner:") != NULL) {
            sscanf(line, "winner:%d", &game->winner);
        } else if(strstr(line, "last_attack:") != NULL) {
            sscanf(line, "last_attack:%d,%d", &game->last_attack_row, &game->last_attack_col);
        } else if(strstr(line, "PLAYER1:") != NULL) {
            sscanf(line, "PLAYER1:%s", game->player1.name);
        } else if(strstr(line, "ship_count:") != NULL && !reading_board1 && !reading_attack1 && !reading_board2 && !reading_attack2) {
            sscanf(line, "ship_count:%d", &game->player1.ship_count);
        } else if(strstr(line, "ships_remaining:") != NULL && !reading_board1 && !reading_attack1 && !reading_board2 && !reading_attack2) {
            sscanf(line, "ships_remaining:%d", &game->player1.ships_remaining);
        } else if(strstr(line, "ship:") != NULL && !reading_board1 && !reading_attack1 && !reading_board2 && !reading_attack2) {
            int idx = 0;
            for(int i = 0; i < game->player1.ship_count; i++) {
                if(game->player1.ships[i].length == 0) { idx = i; break; }
            }
            sscanf(line, "ship:%d,%d,%d,%d,%d,%d", &game->player1.ships[idx].row, &game->player1.ships[idx].col, (int*)&game->player1.ships[idx].length, (int*)&game->player1.ships[idx].direction, &game->player1.ships[idx].hits, &game->player1.ships[idx].is_sunk);
        } else if(strstr(line, "BOARD1:") != NULL) {
            reading_board1 = 1; reading_attack1 = reading_board2 = reading_attack2 = 0; board_row = 0;
        } else if(strstr(line, "ATTACK_BOARD1:") != NULL) {
            reading_attack1 = 1; reading_board1 = reading_board2 = reading_attack2 = 0; board_row = 0;
        } else if(strstr(line, "PLAYER2:") != NULL) {
            sscanf(line, "PLAYER2:%s", game->player2.name);
        } else if(strstr(line, "ship_count:") != NULL && (reading_board1 || reading_attack1)) {
            sscanf(line, "ship_count:%d", &game->player2.ship_count);
        } else if(strstr(line, "ships_remaining:") != NULL && (reading_board1 || reading_attack1)) {
            sscanf(line, "ships_remaining:%d", &game->player2.ships_remaining);
        } else if(strstr(line, "ship:") != NULL && (reading_board1 || reading_attack1)) {
            int idx = 0;
            for(int i = 0; i < game->player2.ship_count; i++) {
                if(game->player2.ships[i].length == 0) { idx = i; break; }
            }
            sscanf(line, "ship:%d,%d,%d,%d,%d,%d", &game->player2.ships[idx].row, &game->player2.ships[idx].col, (int*)&game->player2.ships[idx].length, (int*)&game->player2.ships[idx].direction, &game->player2.ships[idx].hits, &game->player2.ships[idx].is_sunk);
        } else if(strstr(line, "BOARD2:") != NULL) {
            reading_board2 = 1; reading_board1 = reading_attack1 = reading_attack2 = 0; board_row = 0;
        } else if(strstr(line, "ATTACK_BOARD2:") != NULL) {
            reading_attack2 = 1; reading_board1 = reading_attack1 = reading_board2 = 0; board_row = 0;
        } else if(reading_board1 && board_row < BOARD_SIZE) {
            for(int j = 0; j < BOARD_SIZE && j < strlen(line); j++) {
                game->player1.board[board_row][j] = line[j] - '0';
            }
            board_row++;
        } else if(reading_attack1 && board_row < BOARD_SIZE) {
            for(int j = 0; j < BOARD_SIZE && j < strlen(line); j++) {
                game->player1.attack_board[board_row][j] = line[j] - '0';
            }
            board_row++;
        } else if(reading_board2 && board_row < BOARD_SIZE) {
            for(int j = 0; j < BOARD_SIZE && j < strlen(line); j++) {
                game->player2.board[board_row][j] = line[j] - '0';
            }
            board_row++;
        } else if(reading_attack2 && board_row < BOARD_SIZE) {
            for(int j = 0; j < BOARD_SIZE && j < strlen(line); j++) {
                game->player2.attack_board[board_row][j] = line[j] - '0';
            }
            board_row++;
        }
        line = strtok(NULL, "\n");
    }
    printf("Game loaded successfully from '%s'\n", filename);
    return 1;
}
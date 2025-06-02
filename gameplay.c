#include "battleships.h"
#include <stdio.h>
#include <stdlib.h>

void setup_ships_manual(Player* player) {
    printf("\n=== SHIP PLACEMENT FOR %s ===\n", player->name);
    int ship_types[] = {2, 2, 2, 2, 3, 3, 3, 4, 4, 6}; 
    const char* ship_names[] = {"small", "small", "small", "small", 
                               "medium", "medium", "medium", 
                               "large", "large", "cruiser"};
    
    for(int ship_idx = 0; ship_idx < MAX_SHIPS; ship_idx++) {
        int placed = 0;
          while(!placed) {
            printf("\nCurrent board:\n");
            print_board(player->board, 1);
            
            printf("\nPlacing %s ship (length %d)\n", 
                   ship_names[ship_idx], ship_types[ship_idx]);
            
            char col_char;
            int row, direction;
            
            printf("Enter position (e.g. A1): ");
            scanf(" %c%d", &col_char, &row);
            
            int col = get_column_number(col_char);
            row--;
            
            if(row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
                printf("Invalid position! Please try again.\n");
                continue;
            }
            
            printf("Direction (0-horizontal, 1-vertical): ");
            scanf("%d", &direction);
            
            if(direction != 0 && direction != 1) {
                printf("Invalid direction! Please try again.\n");
                continue;
            }
            
            if(place_ship(player, row, col, ship_types[ship_idx], direction)) {
                printf("Ship placed successfully!\n");
                placed = 1;
            } else {
                printf("Invalid position! Ship cannot be placed here.\n");
            }
        }
    }    
    printf("\nAll ships placed!\n");
    print_board(player->board, 1);
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

int make_attack(Player* attacker, Player* defender, int row, int col) {
    if(row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return -1;
    }
    
    if(attacker->attack_board[row][col] != EMPTY) {
        return -1;
    }

    if(defender->board[row][col] == SHIP) {
        defender->board[row][col] = HIT;
        attacker->attack_board[row][col] = HIT;
        
        for(int i = 0; i < defender->ship_count; i++) {
            Ship* ship = &defender->ships[i];
            if(ship->is_sunk) continue;
            
            int ship_hit = 0;
            for(int j = 0; j < ship->length; j++) {
                int ship_row = (ship->direction == VERTICAL) ? ship->row + j : ship->row;
                int ship_col = (ship->direction == HORIZONTAL) ? ship->col + j : ship->col;
                
                if(ship_row == row && ship_col == col) {
                    ship->hits++;
                    ship_hit = 1;
                    break;
                }
            }
            
            if(ship_hit && ship->hits == ship->length) {
                ship->is_sunk = 1;
                defender->ships_remaining--;
                printf("Ship sunk!\n");
                return 2;
            }
        }
        
        return 1;
    } else {
        attacker->attack_board[row][col] = MISS;
        return 0;
    }
}

void start_multiplayer_game() {
    Game game;
    init_game(&game);
    
    printf("Enter first player name: ");
    scanf("%s", game.player1.name);
    init_player(&game.player1, game.player1.name);
    
    printf("Enter second player name: ");
    scanf("%s", game.player2.name);
    init_player(&game.player2, game.player2.name);
    
    setup_ships_manual(&game.player1);
    clear_screen();
    setup_ships_manual(&game.player2);
    clear_screen();
    
    while(!game.game_over) {
        Player* current = (game.current_player == 0) ? &game.player1 : &game.player2;
        Player* opponent = (game.current_player == 0) ? &game.player2 : &game.player1;
        
        printf("\n=== %s's TURN ===\n", current->name);
        printf("Your attack board:\n");
        print_board(current->attack_board, 0);
        
        char col_char;
        int row;
        printf("Enter attack position (e.g. A1): ");
        scanf(" %c%d", &col_char, &row);
        
        int col = get_column_number(col_char);
        row--;
        
        int result = make_attack(current, opponent, row, col);
        
        switch(result) {
            case -1:
                printf("Invalid attack! Try again.\n");
                continue;
            case 0:
                printf("Miss!\n");
                break;
            case 1:
                printf("Hit!\n");
                continue;
            case 2:
                printf("Hit and sink!\n");
                if(opponent->ships_remaining == 0) {
                    game.game_over = 1;
                    game.winner = game.current_player;
                    printf("\n=== %s WINS! ===\n", current->name);
                }
                continue;
        }
        
        game.current_player = 1 - game.current_player;
        
        printf("Press Enter to continue...");
        getchar();
        getchar();
        clear_screen();
    }
}

void start_singleplayer_game() {
    printf("Singleplayer mode not yet implemented.\n");
}

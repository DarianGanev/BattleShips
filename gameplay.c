#include "battleships.h"

void show_ship_setup_menu(Player* player) {
    printf("\n=== SHIP SETUP MENU FOR %s ===\n", player->name);
    printf("1. Place next ship (ships placed: %d/10)\n", player->ship_count);
    printf("2. Edit ship position\n");
    printf("3. View current board\n");
    printf("4. Load from file\n");
    printf("Choose option: ");
}

void setup_ships_manual(Player* player) {
    int ship_types[] = {2, 2, 2, 2, 3, 3, 3, 4, 4, 6};
    const char* ship_names[] = {"small", "small", "small", "small", 
                               "medium", "medium", "medium", 
                               "large", "large", "cruiser"};    while(player->ship_count < MAX_SHIPS) {
        int choice;
        while (1) {
            show_ship_setup_menu(player);
            if (scanf("%d", &choice) == 1) {
                break;
            } else {
                printf("Invalid input! Please enter a number.\n");
                clear_input_buffer();
            }
        }
        
        switch(choice) {
            case 1: {
                printf("\nCurrent board:\n");
                print_board(player->board, 1);
                
                int ship_idx = player->ship_count;
                printf("\nPlacing %s ship (length %d)\n", 
                       ship_names[ship_idx], ship_types[ship_idx]);
                
                char col_char;
                int row, direction;                printf("Enter position (e.g. A1): ");
                while (1) {
                    if (scanf(" %c%d", &col_char, &row) == 2) {
                        break;
                    } else {
                        printf("Invalid input format! Please try again: ");
                        clear_input_buffer();
                    }
                }
                
                int col = get_column_number(col_char);
                row--;
                
                if(row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
                    printf("Invalid position! Please try again.\n");
                    break;
                }                printf("Direction (0-horizontal, 1-vertical): ");
                while (1) {
                    if (scanf("%d", &direction) == 1) {
                        if(direction == 0 || direction == 1) {
                            break;
                        } else {
                            printf("Invalid direction! Please enter 0 or 1: ");
                        }
                    } else {
                        printf("Invalid input! Please enter a number: ");
                        clear_input_buffer();
                    }
                }
                
                if(place_ship(player, row, col, ship_types[ship_idx], direction)) {
                    printf("Ship placed successfully!\n");
                } else {
                    printf("Invalid position! Ship cannot be placed here.\n");
                }
                break;
            }            case 2: {
                if(player->ship_count == 0) {
                    printf("No ships placed yet!\n");
                    break;
                }
                
                printf("\nCurrent board:\n");
                print_board(player->board, 1);
                printf("\nPlaced ships:\n");
                
                for(int i = 0; i < player->ship_count; i++) {
                    Ship* ship = &player->ships[i];
                    printf("%d. Ship at %c%d, length %d, %s\n", 
                           i + 1, 
                           get_column_letter(ship->col), 
                           ship->row + 1,
                           ship->length,
                           ship->direction == HORIZONTAL ? "horizontal" : "vertical");
                }
                
                int ship_to_edit;
                printf("Enter ship number to edit (1-%d): ", player->ship_count);
                while (1) {
                    if (scanf("%d", &ship_to_edit) == 1) {
                        if(ship_to_edit >= 1 && ship_to_edit <= player->ship_count) {
                            break;
                        } else {
                            printf("Invalid ship number! Please enter 1-%d: ", player->ship_count);
                        }
                    } else {
                        printf("Invalid input! Please enter a number: ");
                        clear_input_buffer();
                    }
                }
                
                ship_to_edit--; // Convert to 0-based index
                Ship* old_ship = &player->ships[ship_to_edit];
                
                // Remove old ship from board
                for(int i = 0; i < old_ship->length; i++) {
                    int ship_row = (old_ship->direction == VERTICAL) ? old_ship->row + i : old_ship->row;
                    int ship_col = (old_ship->direction == HORIZONTAL) ? old_ship->col + i : old_ship->col;
                    player->board[ship_row][ship_col] = EMPTY;
                }
                
                // Get new position
                char col_char;
                int row, direction;
                printf("Enter new position (e.g. A1): ");
                while (1) {
                    if (scanf(" %c%d", &col_char, &row) == 2) {
                        break;
                    } else {
                        printf("Invalid input format! Please try again: ");
                        clear_input_buffer();
                    }
                }
                
                int col = get_column_number(col_char);
                row--;
                
                if(row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
                    printf("Invalid position! Restoring original ship placement.\n");
                    // Restore original ship
                    for(int i = 0; i < old_ship->length; i++) {
                        int ship_row = (old_ship->direction == VERTICAL) ? old_ship->row + i : old_ship->row;
                        int ship_col = (old_ship->direction == HORIZONTAL) ? old_ship->col + i : old_ship->col;
                        player->board[ship_row][ship_col] = SHIP;
                    }
                    break;
                }
                
                printf("Direction (0-horizontal, 1-vertical): ");
                while (1) {
                    if (scanf("%d", &direction) == 1) {
                        if(direction == 0 || direction == 1) {
                            break;
                        } else {
                            printf("Invalid direction! Please enter 0 or 1: ");
                        }
                    } else {
                        printf("Invalid input! Please enter a number: ");
                        clear_input_buffer();
                    }
                }
                
                // Try to place ship at new position
                if(is_valid_position(player, row, col, old_ship->length, direction)) {
                    // Update ship position
                    old_ship->row = row;
                    old_ship->col = col;
                    old_ship->direction = direction;
                    
                    // Place ship at new position
                    for(int i = 0; i < old_ship->length; i++) {
                        int ship_row = (direction == VERTICAL) ? row + i : row;
                        int ship_col = (direction == HORIZONTAL) ? col + i : col;
                        player->board[ship_row][ship_col] = SHIP;
                    }
                    
                    printf("Ship moved successfully!\n");
                } else {
                    printf("Invalid position! Restoring original ship placement.\n");
                    // Restore original ship
                    for(int i = 0; i < old_ship->length; i++) {
                        int ship_row = (old_ship->direction == VERTICAL) ? old_ship->row + i : old_ship->row;
                        int ship_col = (old_ship->direction == HORIZONTAL) ? old_ship->col + i : old_ship->col;
                        player->board[ship_row][ship_col] = SHIP;
                    }
                }
                break;
            }
            case 3:
                printf("\nCurrent board:\n");
                print_board(player->board, 1);
                break;            case 4: {
                char filename[256];
                printf("Enter filename (e.g. config.txt): ");
                while (1) {
                    if (scanf("%255s", filename) == 1) {
                        break;
                    } else {
                        printf("Invalid input! Please enter a filename: ");
                        clear_input_buffer();
                    }
                }
                setup_ships_from_file(player, filename);
                if (player->ship_count == MAX_SHIPS) {
                    return;
                }
                break;
            }
            default:
                printf("Invalid choice!\n");
        }
    }
    
    printf("\nAll ships placed!\n");
    print_board(player->board, 1);
    printf("Press Enter to continue...");
    while(getchar() != '\n');
    getchar();
}

void setup_ships_from_file(Player* player, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", filename);
        return;
    }
    
    int ship_types[] = {2, 2, 2, 2, 3, 3, 3, 4, 4, 6};
    int ship_count = 0;
    char line[100];
    
    printf("Loading ship configuration from '%s'...\n", filename);
    
    while (fgets(line, sizeof(line), file) && ship_count < MAX_SHIPS) {
        char col_char;
        int row;
        char direction_char;
        
        if (sscanf(line, "%c%d %c", &col_char, &row, &direction_char) == 3) {
            int col = get_column_number(col_char);
            row--;
            
            Direction direction = (direction_char == 'H' || direction_char == 'h') ? HORIZONTAL : VERTICAL;
            
            if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
                if (place_ship(player, row, col, ship_types[ship_count], direction)) {
                    printf("Ship %d placed at %c%d %s\n", ship_count + 1, 
                           col_char, row + 1, direction == HORIZONTAL ? "horizontal" : "vertical");
                    ship_count++;
                } else {
                    printf("Error: Cannot place ship %d at %c%d - invalid position\n", 
                           ship_count + 1, col_char, row + 1);
                    fclose(file);
                    return;
                }
            } else {
                printf("Error: Invalid coordinates %c%d in file\n", col_char, row + 1);
                fclose(file);
                return;
            }
        } else {
            printf("Error: Invalid line format in file: %s", line);
            fclose(file);
            return;
        }
    }
    
    fclose(file);
    
    if (ship_count == MAX_SHIPS) {
        printf("All ships loaded successfully!\n");
        print_board(player->board, 1);
    } else {
        printf("Error: File contains only %d ships, expected %d\n", ship_count, MAX_SHIPS);
    }
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

void show_attack_menu(Player* current, Player* opponent, Game* game) {
    printf("\n=== %s's TURN ===\n", current->name);
    printf("1. View attack board\n");
    printf("2. Make attack (coordinates)\n");
    printf("3. Make relative attack\n");
    printf("Choose option: ");
}

void start_multiplayer_game(void) {
    Game game;
    init_game(&game);
      printf("Enter first player name: ");
    char name1[50];
    if (scanf("%49s", name1) != 1) {
        printf("Invalid input!\n");
        clear_input_buffer();
        return;
    }
    init_player(&game.player1, name1);
    
    printf("Enter second player name: ");
    char name2[50];
    if (scanf("%49s", name2) != 1) {
        printf("Invalid input!\n");
        clear_input_buffer();
        return;
    }
    init_player(&game.player2, name2);
    
    setup_ships_manual(&game.player1);
    clear_screen();
    setup_ships_manual(&game.player2);
    clear_screen();
    
    while(!game.game_over) {
        Player* current = (game.current_player == 0) ? &game.player1 : &game.player2;
        Player* opponent = (game.current_player == 0) ? &game.player2 : &game.player1;        show_attack_menu(current, opponent, &game);
        
        int choice;
        while (1) {
            if (scanf("%d", &choice) == 1) {
                break;
            } else {
                printf("Invalid input! Please enter a number.\n");
                clear_input_buffer();
                show_attack_menu(current, opponent, &game);
            }
        }
        
        switch(choice) {
            case 1:
                printf("\nYour attack board:\n");
                print_board(current->attack_board, 0);
                break;
            case 2: {                char col_char;
                int row;
                
                while (1) {
                    printf("Enter attack position (e.g. A1): ");
                    if (scanf(" %c%d", &col_char, &row) == 2) {
                        break;
                    } else {
                        printf("Invalid input format! Please try again.\n");
                        clear_input_buffer();
                    }
                }
                
                int col = get_column_number(col_char);
                row--;
                
                int result = make_attack(current, opponent, row, col);
                game.last_attack_row = row;
                game.last_attack_col = col;
                
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
                        if(opponent->ships_remaining == 0) {
                            game.game_over = 1;
                            game.winner = game.current_player;
                            printf("\n=== %s WINS! ===\n", current->name);
                            // Prompt to save the game after win
                            char save_choice;
                            printf("\nWould you like to save this game? (y/n): ");
                            while (1) {
                                if (scanf(" %c", &save_choice) == 1 && (save_choice == 'y' || save_choice == 'n')) break;
                                printf("Please enter 'y' or 'n': ");
                                clear_input_buffer();
                            }
                            if (save_choice == 'y') {
                                char filename[256];
                                char password[64];
                                printf("Enter filename to save (e.g. save1.dat): ");
                                scanf("%255s", filename);
                                printf("Enter password for encryption: ");
                                scanf("%63s", password);
                                save_game_with_password(&game, filename, password);
                            }
                            printf("\nPress Enter to return to main menu...");
                            while(getchar() != '\n');
                            getchar();
                            clear_input_buffer();
                            return;
                        }
                        continue;
                }
                
                game.current_player = 1 - game.current_player;
                printf("Press Enter to continue...");
                while(getchar() != '\n');
                getchar();
                clear_screen();
                break;
            }            case 3: {
                if(game.last_attack_row == -1 || game.last_attack_col == -1) {
                    printf("No previous attack to reference!\n");
                    break;
                }
                
                printf("Previous attack was at %c%d\n", 
                       get_column_letter(game.last_attack_col), 
                       game.last_attack_row + 1);
                
                int direction;
                printf("Select direction from last attack:\n");
                printf("1. Up\n2. Down\n3. Left\n4. Right\n");
                printf("Choose direction: ");
                
                while (1) {
                    if (scanf("%d", &direction) == 1) {
                        if(direction >= 1 && direction <= 4) {
                            break;
                        } else {
                            printf("Invalid direction! Please enter 1-4: ");
                        }
                    } else {
                        printf("Invalid input! Please enter a number: ");
                        clear_input_buffer();
                    }
                }
                
                int target_row = game.last_attack_row;
                int target_col = game.last_attack_col;
                
                switch(direction) {
                    case 1: target_row--; break; // Up
                    case 2: target_row++; break; // Down
                    case 3: target_col--; break; // Left
                    case 4: target_col++; break; // Right
                }
                
                if(target_row < 0 || target_row >= BOARD_SIZE || 
                   target_col < 0 || target_col >= BOARD_SIZE) {
                    printf("Target position is outside the board!\n");
                    break;
                }
                
                if(current->attack_board[target_row][target_col] != EMPTY) {
                    printf("Position already attacked!\n");
                    break;
                }
                
                int result = make_attack(current, opponent, target_row, target_col);
                game.last_attack_row = target_row;
                game.last_attack_col = target_col;
                
                printf("Attack on %c%d: ", get_column_letter(target_col), target_row + 1);
                
                switch(result) {
                    case 0:
                        printf("Miss!\n");
                        break;
                    case 1:
                        printf("Hit!\n");
                        continue;
                    case 2:
                        if(opponent->ships_remaining == 0) {
                            game.game_over = 1;
                            game.winner = game.current_player;
                            printf("\n=== %s WINS! ===\n", current->name);
                            // Prompt to save the game after win
                            char save_choice;
                            printf("\nWould you like to save this game? (y/n): ");
                            while (1) {
                                if (scanf(" %c", &save_choice) == 1 && (save_choice == 'y' || save_choice == 'n')) break;
                                printf("Please enter 'y' or 'n': ");
                                clear_input_buffer();
                            }
                            if (save_choice == 'y') {
                                char filename[256];
                                char password[64];
                                printf("Enter filename to save (e.g. save1.dat): ");
                                scanf("%255s", filename);
                                printf("Enter password for encryption: ");
                                scanf("%63s", password);
                                save_game_with_password(&game, filename, password);
                            }
                            printf("\nPress Enter to return to main menu...");
                            while(getchar() != '\n');
                            getchar();
                            clear_input_buffer();
                            return;
                        }
                        continue;
                }
                
                game.current_player = 1 - game.current_player;
                printf("Press Enter to continue...");
                while(getchar() != '\n');
                getchar();
                clear_screen();
                break;
            }
            default:
                printf("Invalid choice!\n");
        }
    }
}

void start_singleplayer_game(void) {
    Game game;
    init_game(&game);
    
    init_player(&game.player1, "Player");
    init_player(&game.player2, "AI");
    
    clear_screen();
    printf("=== SINGLE PLAYER MODE ===\n\n");
    
    // Player ship setup
    printf("=== PLAYER SHIP SETUP ===\n");
    show_ship_setup_menu(&game.player1);
    
    // AI ship setup
    printf("\n=== AI SHIP SETUP ===\n");
    ai_place_ships(&game.player2);
    
    printf("\nPress Enter to start the battle...");
    getchar();
    clear_input_buffer();
    
    // Game loop
    while(!game.game_over) {
        clear_screen();
        
        if(game.current_player == 0) {
            // Player's turn
            printf("=== YOUR TURN ===\n");
            printf("Your attacks:\n");
            print_board(game.player1.attack_board, 0);
            printf("\nYour board:\n");
            print_board(game.player1.board, 1);
            
            show_attack_menu(&game.player1, &game.player2, &game);
        } else {
            // AI's turn
            printf("=== AI TURN ===\n");
            printf("AI's attacks:\n");
            print_board(game.player2.attack_board, 0);
            
            ai_make_attack(&game.player2, &game.player1, &game);
            
            printf("\nPress Enter to continue...");
            getchar();
            clear_input_buffer();
        }
        
        // Check for winner
        if(game.player1.ships_remaining == 0) {
            game.game_over = 1;
            game.winner = 1; // AI wins
        } else if(game.player2.ships_remaining == 0) {
            game.game_over = 1;
            game.winner = 0; // Player wins
        }
        
        // Switch turns
        if(!game.game_over) {
            game.current_player = 1 - game.current_player;
        }
    }
    
    // Display winner
    clear_screen();
    printf("=== GAME OVER ===\n");
    if(game.winner == 0) {
        printf("Congratulations! You won!\n");
    } else {
        printf("AI wins! Better luck next time!\n");
    }
    printf("\nFinal boards:\n");
    printf("Your board:\n");
    print_board(game.player1.board, 1);
    printf("\nAI's board:\n");
    print_board(game.player2.board, 1);
    // Prompt to save the game
    char save_choice;
    printf("\nWould you like to save this game? (y/n): ");
    while (1) {
        if (scanf(" %c", &save_choice) == 1 && (save_choice == 'y' || save_choice == 'n')) break;
        printf("Please enter 'y' or 'n': ");
        clear_input_buffer();
    }
    if (save_choice == 'y') {
        char filename[256];
        char password[64];
        printf("Enter filename to save (e.g. save1.dat): ");
        scanf("%255s", filename);
        printf("Enter password for encryption: ");
        scanf("%63s", password);
        save_game_with_password(&game, filename, password);
    }
    printf("\nPress Enter to return to main menu...");
    while(getchar() != '\n');
    getchar();
    clear_input_buffer();
}